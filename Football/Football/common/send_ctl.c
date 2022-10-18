/*************************************************************************
	> File Name: send_ctl.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Sun 16 Oct 2022 03:11:01 PM CST
 ************************************************************************/

#include "head.h"

extern int sockfd;
extern struct FootballMsg ctl_msg;

void send_ctl(int signum) {
    if (ctl_msg.ctl.dirx || ctl_msg.ctl.diry) {
        ctl_msg.ctl.action = ACTION_DFT;
        send(sockfd, (void *)&ctl_msg, sizeof(ctl_msg), 0);
        ctl_msg.ctl.dirx = 0;
        ctl_msg.ctl.diry = 0;
        show_data_stream('n'); //正常动作(颜色框)
    }
    return ;
}
