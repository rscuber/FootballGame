/*************************************************************************
	> File Name: udp_server.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Tue 11 Oct 2022 08:22:44 PM CST
 ************************************************************************/

#include "head.h"

extern int listen_port;
extern struct user_passwd *userPasswd;
extern const int max_users;
extern struct User *redTeam;
extern struct User *blueTeam;
extern pthread_mutex_t redMutex;
extern pthread_mutex_t blueMutex;

int socket_create_udp_server(int port) {
    //创建UDP socket
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET; servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // "0.0.0.0"

    make_nonblock(sockfd); //设置为非阻塞

    //设置socket属性: 地址重用
    int reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int));

    //绑定本地ip和端口
    if (bind(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr)) < 0) {
        return -1;
    }
    return sockfd;
}

int udp_connect_client(int epollfd, struct sockaddr_in *clientAddr) {
    int sockfd; 
    //服务端需要绑定端口(服务端需要指定固定ip端口)
    if ((sockfd = socket_create_udp_server(listen_port)) < 0) { 
        perror("socket_udp");
        return -1;
    }
    //connect用在udp上仅仅绑定了客户端和服务端的(ip, port)四元组
    if (connect(sockfd, (struct sockaddr *)clientAddr, sizeof(struct sockaddr)) < 0) {
        DBG(L_RED);
        perror("connect");
        DBG(NONE);
        return -1; 
    }
    DBG(L_YELLOW "Connected to client successfully.\n" NONE);
    return sockfd;
}

int check_online(struct LogRequest *request) {
    for (int i = 0; i < MAX_TEAM_USERS; i++) {
        if (redTeam[i].isOnline && !strcmp(redTeam[i].name, request->name)) return 1; //已经在线
        if (blueTeam[i].isOnline && !strcmp(blueTeam[i].name, request->name)) return 1; //已经在线
    }
    return 0; 
}

int udp_accept(int epollfd, int listener, struct User *user) {
    struct sockaddr_in clientAddr;
    bzero(&clientAddr, sizeof(clientAddr));
    socklen_t len = sizeof(clientAddr);

    struct LogRequest request; //登录请求数据
    struct LogResponse response;//登录反馈数据
    bzero(&request, sizeof(request));
    bzero(&response, sizeof(response));

    int ret = recvfrom(listener, &request, sizeof(request), 0, (struct sockaddr *)&clientAddr, &len);
    if (ret != sizeof(request) || check_user(userPasswd, max_users, &request) < 0) {
        response.type = SIGNIN_FAIL; //登录失败标志
        strcpy(response.msg, "Login failed because of wrong password.\n");
        sendto(listener, &response, sizeof(response), 0, (struct sockaddr *)&clientAddr, len);
        DBG(L_RED "<LoginRequest Fail> " NONE "%s:%d...\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port)); 
        return -1;
    } 

    //检查是否重复登录
    if (check_online(&request)) {
        response.type = SIGNIN_FAIL; //登录失败标志
        strcpy(response.msg, "Login failed because of repeating login.\n");
        sendto(listener, &response, sizeof(response), 0, (struct sockaddr *)&clientAddr, len);
        DBG(L_RED "<LoginRequest Fail> " NONE "%s:%d...\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port)); 
        return -1;
    }

    response.type = SIGNIN_SUCCESS; //登录成功标志
    strcpy(response.msg, "Login successfully. Enjoy yourself!\n");
    sendto(listener, &response, sizeof(response), 0, (struct sockaddr *)&clientAddr, len);
    if (request.team) { //加入蓝队
        DBG(L_YELLOW "<LoginRequest OK> " NONE L_BLUE "%s" NONE " on %s:%d login in and join in the blue team.\n", request.name, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port)); ;
    } else { //加入红队
        DBG(L_YELLOW "<LoginRequest OK> " NONE L_RED "%s" NONE " on %s:%d login in and join in the red team.\n", request.name, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port)); ;
    }

    //填写登录用户的信息
    strcpy(user->name, request.name);
    user->team = request.team;
    user->score = 0;

    int new_fd = udp_connect_client(epollfd, &clientAddr); 
    user->fd = new_fd;
    return new_fd;
}

int findIndex(struct User *team) {
    for (int i = 0; i < MAX_TEAM_USERS; i++) {
        if (!team[i].isOnline) return i;
    } 
    return -1;
}

void add_to_sub_reactor(int sub_epollfd, struct User *user) {
    struct User *team = (user->team ? blueTeam : redTeam);     
    if (user->team) pthread_mutex_lock(&blueMutex);
    else pthread_mutex_lock(&redMutex);
    int index = findIndex(team);
    team[index] = *user; //拷贝一份放在数组中
    team[index].isOnline = 1;
    team[index].lives = 10;
    //初始化登录用户的坐标
    team[index].loc.y = court.height + 1;
    if (user->team) {
        team[index].loc.x = (court.width + 4) / 2 + 2 + 10 + index;
    } else {
        team[index].loc.x = (court.width + 4) / 2 + 2 - 10 - index;
    }
    DBG(L_BLUE "%s[%d] = %s\n" NONE, user->team ? "Blue" : "Red", index, user->name);
    struct FootballMsg r_msg;
    bzero(&r_msg.msg, sizeof(r_msg.msg));
    sprintf(r_msg.msg, "Your friend %s join the game!👏 ", user->name);
    r_msg.type = FT_WALL;
    send_all(&r_msg);
    //边缘触发
    if (user->team) pthread_mutex_unlock(&blueMutex);
    else pthread_mutex_unlock(&redMutex);
    add_events_ptr(sub_epollfd, team[index].fd, EPOLLIN | EPOLLET, &team[index]); 
    return ;
}
