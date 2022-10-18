/*************************************************************************
	> File Name: ui.h
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Mon 10 Oct 2022 11:33:27 PM CST
 ************************************************************************/

#ifndef _GAME_UI_H
#define _GAME_UI_H

#define MSG_WIDTH 70
#define MSG_HEIGHT 18 
#define INFO_WIDTH 10

#define DEFAULT(name, default_value) ((#name[0]) ? (name + 0) : (default_value))
#define Show_Message(arg0, arg1, arg2, arg3) \
    show_message(DEFAULT(arg0, info_win), DEFAULT(arg1, NULL), arg2, DEFAULT(arg3, 0))

extern WINDOW *out_football_win, *football_win, *info_win, *help_win, *score_win, *input_win;
extern struct Map court;

void init_ui();
void gotoxy_putc(int x, int y, char c);
void gotoxy_puts(int x, int y, char *s);
void gotoxy(int x, int y);
void w_gotoxy_putc(WINDOW *win, int x, int y, char c);
void w_gotoxy_puts(WINDOW *win, int x, int y, const char *s);
void destroy_win(WINDOW *win);
WINDOW *create_newwin(int width, int height, int x, int y);
void destroy_ui();
void *draw(void *arg);
void show_message(WINDOW *win, struct User *user, char *msg, int type);

#endif
