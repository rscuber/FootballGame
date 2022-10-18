/*************************************************************************
	> File Name: heart_beat.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Fri 14 Oct 2022 08:16:01 PM CST
 ************************************************************************/

#include "head.h"

extern struct User *redTeam, *blueTeam;
extern int red_reactor, blue_reactor;

void heart_beat_team(struct User *team) {
    struct FootballMsg msg;
    msg.type = FT_HEART;
    for (int i = 0; i < MAX_TEAM_USERS; i++) {
        if (team[i].isOnline) {
            if (!team[i].lives) {
                team[i].isOnline = 0; 
                int sub_epollfd = team[i].team ? blue_reactor : red_reactor;
                del_events(sub_epollfd, team[i].fd);//清除已掉线的客户端
                char buf[128] = {0};
                sprintf(buf, "%s is removed from the game.\n", team[i].name);
                Show_Message( , NULL, buf, 1);
            }
            send(team[i].fd, (void *)&msg, sizeof(msg), 0);
            team[i].lives--; //客户端存活时间减一
        }
    }
    return ;
}

void *heart_beat(void *arg) {
    while (1) {
        sleep(5);
        heart_beat_team(redTeam);
        heart_beat_team(blueTeam);
    }
    return NULL;
}
