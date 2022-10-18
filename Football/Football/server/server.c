/*************************************************************************
	> File Name: server.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Sat 08 Oct 2022 09:31:25 PM CST
 ************************************************************************/

#include "../common/head.h"

const int max_users = 1024;
const char *config = "./server.conf"; //配置文件
const char *passwdfile = "/etc/gamecenter/authz"; //用户密码文件
struct user_passwd *userPasswd;

int listen_port; //listen_port: 登录端口
char server_ip[20] = {0};

struct Map court; //球场
struct Bpoint ball; //球的位置
struct BallStatus ball_status; //球的状态
struct Score score; //比分
char data_stream[20] = {0};

struct User *redTeam; //红方队伍
struct User *blueTeam; //蓝方队伍
int red_reactor, blue_reactor;
pthread_mutex_t redMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t blueMutex = PTHREAD_MUTEX_INITIALIZER;

WINDOW *out_football_win, *football_win, *info_win, *help_win, *score_win, *input_win;

void Usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s -h server_ip -p listen_port!\n", prog_name);
    return ;
} 

void read_cmd_argvs(int argc, char *argv[]) {
    //读取命令行参数
    int opt;
    while ((opt = getopt(argc, argv, "h:p:")) != -1) {
        switch (opt) {
            case 'h': strcpy(server_ip, optarg); break;
            case 'p': listen_port = atoi(optarg); break;
            default: Usage(argv[0]); exit(EXIT_FAILURE);
        }
    }
    //判读是否有多余参数
    argc -= (optind - 1);
    argv += (optind - 1);
    if (argc > 1) {
        Usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    DBG(L_YELLOW "Parse command line arguments end.\n" NONE);
    return ;
}

void read_conf_file() {
    //判断配置文件是否可读
    if (access(config, R_OK)) {
        fprintf(stderr, L_RED "<ERROR> " NONE "config file error!\n");
        exit(EXIT_FAILURE);
    }
    DBG(L_YELLOW "Config file has read mode.\n" NONE);

    //读取配置文件
    if (!strlen(server_ip)) strcpy(server_ip, get_conf_value(config, "BIND-ADDRESS"));
    if (!listen_port) listen_port = atoi(get_conf_value(config, "LISTEN-PORT"));
    DBG(L_BLUE "ip = %s, listen_port = %d\n" NONE, server_ip, listen_port);

    //界面的宽度和高度
    court.width = atoi(get_conf_value(config, "WIDTH"));
    court.height = atoi(get_conf_value(config, "HEIGHT"));
    court.start.x = 3; //球场的起始位置
    court.start.y = 3;

    ball.x = court.width / 2;//球的起始位置
    ball.y = court.height / 2;

    //初始化球状态信息
    bzero(&ball_status, sizeof(ball_status));
    ball_status.by_team = -1;
    bzero(&score, sizeof(score));

    DBG(L_YELLOW "Read config file end.\n" NONE);
    return ;
}

void get_user_passwd() {
    //读取密码到内存
    userPasswd = (struct user_passwd *)calloc(max_users, sizeof(struct user_passwd));
    if (read_passwd(passwdfile, userPasswd, max_users) < 0) {
        handle_error("read_passwd");
    } 
    DBG(L_YELLOW "Username and passwd in %s lists.\n" NONE, passwdfile);
    for (int i = 0; i < max_users; i++) {
        if (!userPasswd[i].flag) continue;
        DBG(L_BLUE "username = %s, passwd = %s\n" NONE, userPasswd[i].username, userPasswd[i].passwd);
    }
    return;
}

void send_interval_redraw_msg() {
    signal(SIGALRM, redraw);
    struct itimerval itimer;
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = 100000; //每10万微秒重绘一次
    itimer.it_value.tv_sec = 3; //服务端启动3秒后开始间隔发送重绘信息
    itimer.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itimer, NULL);
    return ;
}

int main(int argc, char *argv[]) {
    int listener, main_reactor;//监听套接字，主反应堆epollfd
    pthread_t draw_tid, red_tid, blue_tid, heart_tid;
    read_cmd_argvs(argc, argv); //读取命令行参数
    read_conf_file(); //读取配置文件
    get_user_passwd(); //读取密码到内存

    //可动态按需申请
    //两个队伍
    redTeam = (struct User *)calloc(MAX_TEAM_USERS, sizeof(struct User));
    blueTeam = (struct User *)calloc(MAX_TEAM_USERS, sizeof(struct User));

    //创建listener的UDP套接字
    if ((listener = socket_create_udp_server(listen_port)) < 0) {
        handle_error("socket_udp"); 
    }
    DBG(L_YELLOW "Created UDP socket successfully.\n" NONE);

    #ifndef DEBUG
    //服务端不需要绘制游戏界面，只需要将计算结果发送给客户端，客户端完成本地再现
    pthread_create(&draw_tid, NULL, draw, NULL);
    //init_ui();
    #endif

    //创建主从反应堆
    main_reactor = epoll_create(MAX_EVENTS * 2);
    red_reactor = epoll_create(MAX_EVENTS);
    blue_reactor = epoll_create(MAX_EVENTS);
    if (main_reactor < 0 || red_reactor < 0 || blue_reactor < 0) {
        handle_error("epoll_create");
    }

    //创建任务队列
    struct task_queue redQueue;
    struct task_queue blueQueue;
    task_queue_init(&redQueue, MAX_TASKS, red_reactor);
    task_queue_init(&blueQueue, MAX_TASKS, blue_reactor);

    //从反应堆
    pthread_create(&red_tid, NULL, sub_reactor, (void *)&redQueue);
    pthread_create(&blue_tid, NULL, sub_reactor, (void *)&blueQueue);
    pthread_create(&heart_tid, NULL, heart_beat, NULL);

    signal(SIGINT, server_exit); //ctrl+c为退出

    struct epoll_event ev, events[MAX_EVENTS];
    ev.data.fd = listener;
    ev.events = EPOLLIN;
    if (epoll_ctl(main_reactor, EPOLL_CTL_ADD, listener, &ev) < 0) {
        handle_error("epoll_ctl");
    }
    DBG(L_YELLOW "Created main reactor and added listener to it successfully.\n" NONE);

    send_interval_redraw_msg(); //发送游戏画面

    Show_Message( , , "Waiting for login...\n", 1);
    //使epoll屏蔽时钟信号
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGALRM);
    for (;;) {
        //主反应堆
        int nfds = epoll_pwait(main_reactor, events, MAX_EVENTS * 2, -1, &sigset); 
        if (nfds <= 0) handle_error("epoll_wait");
        DBG(L_YELLOW "<Main Reactor> " NONE "Something happens.\n");
        for (int i = 0; i < nfds; i++) {
            char buf[512] = {0};
            struct User user;
            bzero(&user, sizeof(user));
            int fd = events[i].data.fd;
            if (fd == listener) {
                int new_fd = udp_accept(main_reactor, fd, &user);
                if (new_fd > 0) {
                    //添加到对应的从反应堆
                    sprintf(buf, "%s login the game.\n", user.name);
                    Show_Message( , , buf, 1);
                    show_data_stream('l');
                    DBG(L_YELLOW "<MainThread> " NONE "Add %s to the %s sub_reactor.\n", user.name, user.team ? "blue" : "red");
                    int sub_epollfd = user.team ? blue_reactor : red_reactor;
                    add_to_sub_reactor(sub_epollfd, &user);
                }
            } else {
                recv(fd, (void *)buf, sizeof(buf), 0); 
                DBG(L_RED "<Recv> " NONE "%s\n", buf);
            }
        }
    }
    return 0;
}
