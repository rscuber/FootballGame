/*************************************************************************
	> File Name: ball_status.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Sun 16 Oct 2022 10:51:53 PM CST
 ************************************************************************/

#include "head.h"

extern WINDOW *info_win;
extern struct Bpoint ball;
extern struct BallStatus ball_status;

int can_kick(struct Point *loc, int strength) {
    char temp[512] = {0};
    int lx = loc->x - 2, ly = loc->y - 1; //两个窗口有偏移
    sprintf(temp, "loc->x = %d, loc->y = %d, (int)ball.x = %d, (int)ball.y = %d", lx, ly, (int)ball.x, (int)ball.y);
    Show_Message(info_win, NULL, temp, 1);
    if (abs(lx - (int)ball.x) <= 3 && abs(ly - (int)ball.y) <= 3) {
        double v_tmp = (strength * 40.0) * 0.2; //初速度
        sprintf(temp, "v_tmp = %lf", v_tmp);
        Show_Message(info_win, NULL, temp, 1);
        double angle;
        //计算角度
        if (lx == (int)ball.x) {
            angle = PI / 2; 
        } else {
            angle = atan((fabs(ly - ball.y)) / fabs(lx - ball.x));
        }
        //计算分速度，分加速度
        //球运动时的加速度是由阻力产生的(这里阻力造成的加速度设为3)
        if (lx > ball.x && ly > ball.y) {
            ball_status.v.x = -fabs(cos(angle)) * v_tmp; 
            ball_status.v.y = -fabs(sin(angle)) * v_tmp;
            ball_status.a.x = fabs(cos(angle)) * 3;
            ball_status.a.y = fabs(sin(angle)) * 3;
        } else if (lx > ball.x && ly < ball.y) {
            ball_status.v.x = -fabs(cos(angle)) * v_tmp; 
            ball_status.v.y = fabs(sin(angle)) * v_tmp;
            ball_status.a.x = fabs(cos(angle)) * 3;
            ball_status.a.y = -fabs(sin(angle)) * 3;
        } else if (lx < ball.x && ly > ball.y) {
            ball_status.v.x = fabs(cos(angle)) * v_tmp; 
            ball_status.v.y = -fabs(sin(angle)) * v_tmp;
            ball_status.a.x = -fabs(cos(angle)) * 3;
            ball_status.a.y = fabs(sin(angle)) * 3;
        } else {
            ball_status.v.x = fabs(cos(angle)) * v_tmp; 
            ball_status.v.y = fabs(sin(angle)) * v_tmp;
            ball_status.a.x = -fabs(cos(angle)) * 3;
            ball_status.a.y = -fabs(sin(angle)) * 3;
        }
        return 1;
    }
    return 0;
}

int can_access(struct Point *loc) {
    char temp[512] = {0};
    int lx = loc->x - 2, ly = loc->y - 1; //两个窗口有偏移
    sprintf(temp, "loc->x = %d, loc->y = %d, (int)ball.x = %d, (int)ball.y = %d", lx, ly, (int)ball.x, (int)ball.y);
    Show_Message(info_win, NULL, temp, 1);
    if (abs(lx - (int)ball.x) <= 2 && abs(ly - (int)ball.y) <= 2) return 1;
    return 0;
}
