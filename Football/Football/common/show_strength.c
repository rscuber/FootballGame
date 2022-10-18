/*************************************************************************
	> File Name: show_strength.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Mon 17 Oct 2022 04:44:04 PM CST
 ************************************************************************/

#include "head.h"

extern WINDOW *input_win;
extern int sockfd;

//客户端中，按下空格键，调用此函数
////在input_win窗口中，显示踢球力度条，光标在进度条上快速移动
//设置0(stdin)文件为非阻塞IO
//while 等待空格或者'k'键的按下，如果按下退出，取得当前的strength
//通过sockfd向服务端发送控制信息，踢球
void show_strength() {
    int maxx, maxy, strength;
    getmaxyx(input_win, maxy, maxx);
    DBG("maxx = %d, maxy = %d\n", maxx,maxy);
    struct FootballMsg msg;
    bzero(&msg, sizeof(msg));
    for (int i = 2; i < maxx - 2; i++) {
        if (i < maxx / 5 || i > (maxx - maxx / 5)) {
            wattron(input_win, COLOR_PAIR(9));
        } else if (i < 2 * (maxx / 5) || i > 3 * (maxx / 5)) {
            wattron(input_win, COLOR_PAIR(12));
        } else {
            wattron(input_win, COLOR_PAIR(8));
        }
        mvwaddch(input_win, 2, i, ' ');
    }
    wattron(input_win, COLOR_PAIR(7));
    //力度(亮级)数组
    int light[5] = {1, 2, 3, 2, 1};
    int offset = 1;
    int tmp_x = 2;
    make_nonblock(0);
    while (1) {
        int c = getchar(); 
        if (c != -1) {
            if (c == ' ' || c == 'k') {
                mvwaddch(input_win, 1, tmp_x, ' '); //清除上下光标
                mvwaddch(input_win, 3, tmp_x, ' ');
                make_block(0);
                break;
            }
        }
        usleep(20000);
        mvwaddch(input_win, 1, tmp_x, ' ');
        mvwaddch(input_win, 3, tmp_x, ' ');
        tmp_x += offset;
        mvwaddch(input_win, 1, tmp_x, '|');
        mvwaddch(input_win, 3, tmp_x, '|');
        mvwaddch(input_win, 4, maxx, ' ');
        //滚动越界处理
        if (tmp_x >= maxx - 2) offset = -1;
        if (tmp_x <= 2) offset = 1;
        wrefresh(input_win); 
    }
    char info[512] = {0};
    sprintf(info, "strength = %d\n", light[tmp_x / (maxx / 5)]);
    Show_Message( , NULL, info, 1);
    msg.type = FT_CTL;
    msg.ctl.action = ACTION_KICK;
    msg.ctl.strength = light[tmp_x / (maxx / 5)];
    send(sockfd, (void *)&msg, sizeof(msg), 0);
    return ;
}
