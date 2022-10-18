/*************************************************************************
	> File Name: send_all.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Mon 17 Oct 2022 05:27:01 PM CST
 ************************************************************************/

#include "head.h"
extern struct User *redTeam;
extern struct User *blueTeam;

void send_all(struct FootballMsg *msg) {
    for (int i = 0; i < MAX_TEAM_USERS; i++) {
        if (redTeam[i].isOnline) { 
            send(redTeam[i].fd, (void *)msg, sizeof(struct FootballMsg), 0);
        }
        if (blueTeam[i].isOnline) { 
            send(blueTeam[i].fd, (void *)msg, sizeof(struct FootballMsg), 0);
        }
    }
    return ;
}
