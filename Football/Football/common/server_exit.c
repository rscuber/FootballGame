/*************************************************************************
	> File Name: server_exit.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Fri 14 Oct 2022 10:33:20 PM CST
 ************************************************************************/

#include "head.h"

extern struct User *redTeam, *blueTeam;

void server_exit(int signum) {
    struct FootballMsg msg;
    bzero(&msg, sizeof(msg));
    msg.type = FT_FIN;
    for (int i = 0; i < MAX_TEAM_USERS; i++) {
        if (redTeam[i].isOnline) send(redTeam[i].fd, (void *)&msg, sizeof(msg), 0); 
        if (blueTeam[i].isOnline) send(blueTeam[i].fd, (void *)&msg, sizeof(msg), 0); 
    }
    endwin();
    DBG(L_PINK "Server stopped!\n" NONE);
    exit(0);
    return ;
}
