/*************************************************************************
	> File Name: redraw.h
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Sun 16 Oct 2022 03:54:39 PM CST
 ************************************************************************/

#ifndef _REDRAW_H
#define _REDRAW_H

void redraw_player(int team, const char *name, struct Point *loc);
void redraw_team(struct User *team);
void redraw_gate();
void redraw_ball();
void redraw(int signum);

#endif
