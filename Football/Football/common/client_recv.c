/*************************************************************************
	> File Name: client_recv.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Fri 14 Oct 2022 09:43:49 PM CST
 ************************************************************************/

#include "head.h"

extern int sockfd;

void *client_recv(void *arg) {
    while (1) {
        //收数据
        struct FootballMsg msg; 
        bzero(&msg, sizeof(msg));
        struct User user;
        bzero(&user, sizeof(user));
        ssize_t rsize = recv(sockfd, (void *)&msg, sizeof(msg), 0);
        if (rsize != sizeof(msg)) {
            DBG(L_RED "Rsize error!\n" NONE);
            continue;
        }
        strcpy(user.name, msg.name);
        user.team = msg.team;
        if (msg.type & FT_HEART) {
            DBG(L_RED "Heart beat 💗 from server." NONE);
            msg.type = FT_ACK;
            send(sockfd, (void *)&msg, sizeof(msg), 0);
        } else if (msg.type & FT_MSG) {
            DBG(L_BLUE "<ServerMsg> " NONE "%s\n", msg.msg); 
            Show_Message( , &user, msg.msg, 0);
        } else if (msg.type & FT_WALL) {
            DBG(L_PINK "Recvive FT_WALL msg.\n" NONE);
            Show_Message( , NULL, msg.msg, 1);
        } else if (msg.type & FT_FIN) {
            DBG(L_PINK "Server is going to stop!👋\n" NONE);
            close(sockfd);
            endwin();
            exit(0);
        } else if (msg.type & FT_MAP) {
            DBG(L_PINK "Recvive FT_MAP msg.\n" NONE);
            Show_Message( , NULL, "Football game refresh.\n", 0);
            //parse_spirit(msg.msg, msg.size); 
        } else {
            DBG(L_RED "<ServerMsg> " NONE "Unsupported message type.\n ");
        }
    }
    return NULL;
}
