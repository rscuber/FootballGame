/*************************************************************************
	> File Name: client.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Sun 09 Oct 2022 06:15:27 PM CST
 ************************************************************************/

#include "../common/head.h"

const char *config = "./game.conf";
int server_port;
char server_ip[20] = {0};
int sockfd;

struct Map court; //球场
struct Bpoint ball; //球的位置
struct BallStatus ball_status; //球的状态
struct Score score; //比分
char data_stream[20] = {0};

WINDOW *out_football_win, *football_win, *info_win, *help_win, *score_win, *input_win;

struct LogRequest request;
struct LogResponse response;
struct FootballMsg chat_msg;
struct FootballMsg ctl_msg;

void init_msg() {
    bzero(&request, sizeof(request));
    bzero(&response, sizeof(response));
    request.team = -1;
    bzero(&chat_msg, sizeof(chat_msg));
    bzero(&ctl_msg, sizeof(ctl_msg));
    chat_msg.type = FT_MSG;
    ctl_msg.type = FT_CTL;
    return ;
}

void Usage(const char *prog_name) {
    fprintf(stderr, "%s -h server_ip -p server_port -u username -p passwd -k key -t token\n", prog_name);
    return ;
}

void get_cmd_argvs(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "h:p:n:P:t:m")) != -1) {
        switch (opt) {
            case 'h': strcpy(server_ip, optarg); break;
            case 'p': server_port = atoi(optarg); break;
            case 'n': strcpy(request.name, optarg); break;
            case 'P': strcpy(request.passwd, optarg); break;
            case 't': request.team = atoi(optarg); break;
            case 'm': strcpy(request.msg, optarg); break;
            default: Usage(argv[0]); exit(EXIT_FAILURE);
        } 
    }
    DBG(L_YELLOW "Parse command line arguments end.\n" NONE);
    return ;
}

void read_conf_file() {
    //判断配置文件是否可读
    if (access(config, R_OK)) {
        fprintf(stderr, L_RED "<ERROR> " NONE "Config file error!\n");
        exit(EXIT_FAILURE);
    }
    DBG(L_YELLOW "Config file has read mode.\n" NONE);

    //读取配置文件
    if (!strlen(server_ip)) strcpy(server_ip, get_conf_value(config, "SERVER-IP"));
    if (!server_port) server_port = atoi(get_conf_value(config, "SERVER-PORT"));
    if (!strlen(request.name)) strcpy(request.name, get_conf_value(config, "NAME"));
    if (!strlen(request.passwd)) strcpy(request.passwd, get_conf_value(config, "PASSWORD"));
    if (request.team == -1) request.team = atoi(get_conf_value(config, "TEAM"));
    DBG(L_YELLOW "Read config file end.\n" NONE);

    DBG(L_BLUE "server_ip = %s, server_port = %d, name = %s, passwd = %s, team = %s\n" NONE, \
    server_ip, server_port, request.name, request.passwd, request.team ? "BLUE" : "RED");

    //界面的宽度和高度
    court.width = atoi(get_conf_value(config, "WIDTH"));
    court.height = atoi(get_conf_value(config, "HEIGHT"));
    court.start.x = 3;
    court.start.y = 3;
    return ;
}

int sign_in(int sockfd, char *name, char *passwd) {
login:
    //输入用户名和密码
    while (!strlen(name) || !strlen(passwd)) {
        fprintf(stderr, L_CYAN "Please sign in...\n" NONE);
        printf(L_GREEN "Name:" NONE);
        fflush(stdout);
        scanf("%[^\n]s", name);
        getchar();
        printf(L_GREEN "Password:" NONE);
        fflush(stdout);
        scanf("%[^\n]s", passwd);
        getchar();
    }
    DBG(L_BLUE "name = %s, passwd = %s\n" NONE, name, passwd);

    //填充登录信息
    struct LogRequest signin_data;
    bzero(&signin_data, sizeof(signin_data));
    signin_data.type = SIGNIN;
    strcpy(signin_data.name, name);
    strcpy(signin_data.passwd, passwd);
    //发送登录包
    send(sockfd, (void *)&signin_data, sizeof(struct LogRequest), 0); 
    DBG(L_BLUE "<LoginRequest> " NONE "Sent login request data.\n");

    //限制服务端响应时间(用select限制登录时间为2秒)
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    if (select(sockfd + 1, &rfds, NULL, NULL, &tv) <= 0) {
        //登录超时
        DBG(L_RED "<LoginRequest> " NONE "Client has not received login response data within 2 sec.\n");
        return -1;
    }

    //接受服务端发送的登录反馈的数据
    struct LogResponse response_data;
    int ret = recv(sockfd, (void *)&response_data, sizeof(response_data), 0);
    if (ret <= 0 || ret != sizeof(response_data)) {
        DBG(L_RED "Recv error!\n" NONE);
        return -1;
    } else {
        DBG(L_YELLOW "Recv %d bytes data!\n" NONE, ret);
    }
    if (response_data.type == SIGNIN_SUCCESS) {//登录成功
        DBG(L_BLUE "%s\n" NONE, response_data.msg);
    } else {
        //登录失败，重新登录
        DBG(L_RED "%s\n" NONE, response_data.msg);
        bzero(name, sizeof(char) * 20);
        bzero(passwd, sizeof(char) * 20);
        goto login;//重新输入账户密码
    }
    DBG(L_BLUE "<Connected> " NONE "%s:%d\n", server_ip, server_port);
    return 0;
}

void logout(int signum) { // SIGINT处理函数(用于客户端退出游戏)
    struct FootballMsg msg; 
    bzero(&msg, sizeof(msg));
    msg.type = FT_FIN;
    send(sockfd, (void *)&msg, sizeof(msg), 0);
    endwin();
    exit(0);
    return ;
}
    
void send_interval_ctl_msg() {
    signal(SIGALRM, send_ctl);
    struct itimerval itimer;
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = 100000; //十万微妙，100毫秒
    itimer.it_value.tv_sec = 0;
    itimer.it_value.tv_usec = 100000;
    setitimer(ITIMER_REAL, &itimer, NULL);
    return ;
}

int main(int argc, char *argv[]) {
    init_msg(); //初始化发送信息
    get_cmd_argvs(argc, argv); //获取命令行参数
    read_conf_file(); //读取配置文件

    //连接游戏服务端
    if ((sockfd = socket_connect_udp_game(server_ip, server_port)) < 0) {
        handle_error("socket_udp"); 
    }
    DBG(L_YELLOW "<%d> connect to server successfully.\n" NONE, sockfd);

    //登录账户
    sign_in(sockfd, request.name, request.passwd);

    signal(SIGINT, logout); //ctrl+c 退出游戏

    #ifndef DEBUG
    pthread_t draw_tid; //绘制图形界面的线程
    pthread_create(&draw_tid, NULL, draw, NULL);
    //init_ui();
    #endif

    pthread_t recv_tid; //收数据的线程
    pthread_create(&recv_tid, NULL, client_recv, NULL);

    send_interval_ctl_msg(); //每隔十万微妙发送控制信息
    //keypad(input_win, TRUE);
    while (1) {
        noecho();
        cbreak();
        int c = getchar(); 
        switch (c) {
            case 'A':
            case 'a': ctl_msg.ctl.dirx -= 1; break;
            case 'D':
            case 'd': ctl_msg.ctl.dirx += 1; break;
            case 'W':
            case 'w': ctl_msg.ctl.diry -= 1; break;
            case 'S':
            case 's': ctl_msg.ctl.diry += 1; break;
            case '@': send_chat(); break;
            case ' ': show_strength(); break;
            case 'k': {
                show_data_stream('k');
                struct FootballMsg msg; 
                bzero(&msg, sizeof(msg));
                msg.type = FT_CTL;
                msg.ctl.action = ACTION_KICK;
                msg.ctl.strength = 1;
                send(sockfd, (void *)&msg, sizeof(msg), 0);
            } break;
        }
    }
    return 0;
}
