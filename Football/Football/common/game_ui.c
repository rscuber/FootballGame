/*************************************************************************
	> File Name: game_ui.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Mon 10 Oct 2022 11:32:37 PM CST
 ************************************************************************/

#include "head.h"

int msgcnt;
extern struct Bpoint ball;
extern struct BallStatus ball_status;

//所有cueses函数使用的坐标都是y值(行号)在前，x值(列号)在后。
WINDOW *create_newwin(int width, int height, int startx, int starty) {
    WINDOW *win;
    win = newwin(height, width, starty, startx);
    box(win, 0, 0); //绘制边框
    wrefresh(win);  //刷新窗口
    return win;
}

void destroy_win(WINDOW *win) {
    wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');//清除box画出的边框
    wrefresh(win);
    delwin(win);
    return ;
}

void gotoxy(int x, int y) {
    move(y, x);
    return ;
}

void gotoxy_putc(int x, int y, char c) {
    move(y, x);
    addch(c);
    move(LINES - 1, 1);
    refresh();
    return ;
}

void gotoxy_puts(int x, int y, char *s) {
    move(y, x);
    addstr(s);
    move(LINES - 1, 1);
    refresh();
    return ;
}

void w_gotoxy_putc(WINDOW *win, int x, int y, char c) {
    //指定了窗口
    mvwaddch(win, y, x, c);     
    move(LINES - 1, 1);
    wrefresh(win);
    return ;
}

void w_gotoxy_puts(WINDOW *win, int x, int y, const char *s) {
    //mvwaddstr(win, y, x, s);     
    mvwprintw(win, y, x, s);
    move(LINES - 1, 1);
    wrefresh(win);
    return ;
}

void init_ui() {
    initscr();
    if (!has_colors() || start_color() == ERR) {
        //终端不支持颜色或者初始化了颜色显示功能失败
        endwin();
        fprintf(stderr, "Iterm do not have colors.\n");
        exit(EXIT_FAILURE);
    } 
    //初始化颜色对
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK); //消除颜色
    init_pair(8, COLOR_BLACK, COLOR_RED);
    init_pair(9, COLOR_BLACK, COLOR_GREEN);
    init_pair(10,COLOR_BLACK,  COLOR_YELLOW);
    init_pair(11,COLOR_BLACK,  COLOR_BLUE);
    init_pair(12,COLOR_BLACK,  COLOR_MAGENTA);
    init_pair(13,COLOR_BLACK,  COLOR_CYAN);

    //创建窗口并记录
    out_football_win = create_newwin(court.width + 4, court.height + 2, court.start.x - 2, court.start.y - 1);//游戏窗口
    football_win = subwin(out_football_win, court.height, court.width, court.start.y, court.start.x);//游戏窗口

    WINDOW *out_info_win = create_newwin(court.width + 4, 7, court.start.x - 2, court.start.y + court.height + 1);//信息窗口
    info_win = subwin(out_info_win, 5, court.width + 2, court.start.y + court.height + 2, court.start.x - 1);//信息子窗口

    scrollok(info_win, 1); //设置为可滚动
    help_win = create_newwin(20, court.height + 2, court.start.x + court.width + 2, court.start.y - 1);//帮助手册窗口
    score_win = create_newwin(20, 7, court.start.x + court.width + 2, court.start.y + court.height + 1);//成绩窗口
    input_win = create_newwin(court.width + 4 + 20, 5, court.start.x - 2, court.start.y + 1 + court.height + 7);//输入窗口
    box(input_win, 0, 0);
    wattron(football_win, COLOR_PAIR(4));
    w_gotoxy_putc(football_win, ball.x, ball.y, 'o');

    wattron(help_win, COLOR_PAIR(3));
    w_gotoxy_puts(help_win, 8, 2, "Help");
    w_gotoxy_puts(help_win, 3, 5, "W - Move Up");
    w_gotoxy_puts(help_win, 3, 6, "S - Move Down");
    w_gotoxy_puts(help_win, 3, 7, "A - Move Left");
    w_gotoxy_puts(help_win, 3, 8, "D - Move Right");
    w_gotoxy_puts(help_win, 3, 10, "J - Stop Ball");
    w_gotoxy_puts(help_win, 3, 11, "K - Kick Ball");
    w_gotoxy_puts(help_win, 3, 12, "L - Carry Ball");
    w_gotoxy_puts(help_win, 3, 14, "Space - Strength");
    wattron(help_win, COLOR_PAIR(7));
    wattron(score_win, COLOR_PAIR(5));
    w_gotoxy_puts(score_win, 7, 1, "Score");
    wattron(score_win, COLOR_PAIR(7));
    return ;
}

void *draw(void *arg) {
    init_ui();
    while (1) {
        sleep(10);
    }
    return NULL;
}

void destroy_ui() {
    destroy_win(football_win);
    destroy_win(help_win);
    destroy_win(info_win);
    destroy_win(score_win);
    destroy_win(input_win);
    endwin();
    return ;
}

void show_message(WINDOW *win, struct User *user, char *msg, int type) {
    //展示消息和时间
    time_t time_now = time(NULL);
    struct tm *tm = localtime(&time_now);
    char timestr[10] = {0};
    sprintf(timestr, "%02d:%02d:%02d ", tm->tm_hour, tm->tm_min, tm->tm_sec);
    char username[100] = {0};
    //处理不同种类信息
    if (type) {
        //处理系统提示信息
        wattron(win, COLOR_PAIR(3));
        strcpy(username, "SysInfo: ");
    } else {
        //处理用户消息
        if (user->team) {
            wattron(win, COLOR_PAIR(4));
        } else {
            wattron(win, COLOR_PAIR(1));
        }
        sprintf(username, "%s: ", user->name);
    }
    if (msgcnt < 4) {
        w_gotoxy_puts(win, 10, msgcnt, username);
        //消除颜色
        wattron(win, COLOR_PAIR(7));
        w_gotoxy_puts(win, 10 + strlen(username), msgcnt, msg);
        wattron(win, COLOR_PAIR(6));
        w_gotoxy_puts(win, 1, msgcnt, timestr);
        msgcnt++;
    } else {
        msgcnt = 4;
        scroll(win); //滚屏
        w_gotoxy_puts(win, 10, msgcnt, username);
        wattron(win, COLOR_PAIR(7)); //消除颜色
        w_gotoxy_puts(win, 10 + strlen(username), msgcnt, msg);//在名字后面输出消息
        wattron(win, COLOR_PAIR(6));
        w_gotoxy_puts(win, 1, msgcnt, timestr);//输出时间
        msgcnt++;
    }
    wrefresh(win);//刷新窗口

    //消息可能很长，需要计算消息所占行数
    // 11为时间字符长度
    int temp = (strlen(msg) + 11 + strlen(username)) / (court.width - 2) + 1;
    msgcnt += temp;
    wmove(input_win, 2, 1);
    wrefresh(input_win);
    return ;
}
