/*************************************************************************
	> File Name: ball_status.h
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Sun 16 Oct 2022 10:51:56 PM CST
 ************************************************************************/

#ifndef _BALL_STATUS_H
#define _BALL_STATUS_H

#define PI acos(-1)

int can_kick(struct Point *loc, int strength);
int can_access(struct Point *loc);

#endif
