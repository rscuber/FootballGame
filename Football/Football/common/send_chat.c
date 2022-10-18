/*************************************************************************
	> File Name: send_chat.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Sun 16 Oct 2022 11:16:39 AM CST
 ************************************************************************/

#include "head.h"

extern int sockfd;
extern WINDOW *input_win;
extern struct FootballMsg chat_msg;

void send_chat() {
    echo(); //设置回显
    nocbreak(); //行缓冲(按回车键提交信息)
    bzero(chat_msg.msg, sizeof(chat_msg.msg));
    chat_msg.type = FT_WALL;
    DBG(L_CYAN "Please input:" NONE);
    w_gotoxy_puts(input_win, 1, 1, "Input Message:");
    mvwscanw(input_win, 2, 1, "%[^\n]s", chat_msg.msg);
    if (strlen(chat_msg.msg)) send(sockfd, (void *)&chat_msg, sizeof(chat_msg), 0);
    wclear(input_win); //清空输入框
    box(input_win, 0, 0);
    wrefresh(input_win);
    noecho();
    cbreak; //恢复原属性
    return ;
}
