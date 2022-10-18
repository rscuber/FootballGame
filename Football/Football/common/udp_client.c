/*************************************************************************
	> File Name: udp_client.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Thu 13 Oct 2022 03:37:43 PM CST
 ************************************************************************/

#include "head.h"

int socket_create_udp_client() {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }
    return sockfd;
}

int socket_connect_udp_game(const char *ip, int port) {
    int sockfd; 
    //客户端不需要绑定端口(绑定端口的话很可能被占用)
    if ((sockfd = socket_create_udp_client()) < 0) { 
        return -1;
    }
    struct sockaddr_in servAddr; 
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = inet_addr(ip);
    //connect用在udp上仅仅绑定了(ip, port)四元组
    if (connect(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        return -1; 
    }

    /*
    send(sockfd, (void *)loginRequest, sizeof(struct LogRequest), 0);
    DBG(L_BLUE "<LoginRequest Sent> " NONE "%s:%d...\n", ip, port);

    //用select限制登录时间(1秒)
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (select(sockfd, &rfds, NULL, NULL, &tv) < 0) {
        //登录超时
        DBG(L_RED "<LoginRequest TimeOut>" NONE "%s:%d...", ip, port);
        return -1; 
    }
    struct LogResponse loginResponse;
    bzero(&loginResponse, sizeof(loginResponse));
    int ret = recv(sockfd, &loginResponse, sizeof(loginResponse), 0);
    if (ret != sizeof(loginResponse) || loginResponse.type & SIGNIN_FAIL == 0) {
        //登录失败
        DBG(L_RED "<LoginRequest Error> " NONE "%s:%d...", ip, port);
        return -1;
    }
    DBG(L_GREEN "<Connected> " NONE "%s:%d\n", ip, port);
    */
    return sockfd;
}

