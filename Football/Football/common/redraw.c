/*************************************************************************
	> File Name: redraw.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Sun 16 Oct 2022 03:54:34 PM CST
 ************************************************************************/

#include "head.h"

extern int sockfd;
extern struct User *redTeam;
extern struct User *blueTeam;
extern WINDOW *football_win;
extern struct Bpoint ball;
extern struct BallStatus ball_status;
extern struct Score score;

void redraw_player(int team, const char *name, struct Point *loc) {
    char p = 'K'; //表示球员
    if (team) wattron(football_win, COLOR_PAIR(4));
    else wattron(football_win, COLOR_PAIR(1));
    w_gotoxy_putc(football_win, loc->x, loc->y, p); //重绘球员
    w_gotoxy_puts(football_win, loc->x, loc->y - 1, name);//在球员上面加上名字
    return ;
}

void redraw_team(struct User *team) {
    for (int i = 0; i < MAX_TEAM_USERS; i++) {
        if (!team[i].isOnline) continue; 
        redraw_player(team[i].team, team[i].name, &team[i].loc); 
    }
    return ;
}

void redraw_gate() {
    wattron(out_football_win, COLOR_PAIR(6));
    int rx = court.width + 2, lx = 1;
    for (int y = court.height / 2 - 3; y <= court.height / 2 + 3; y++) {
        w_gotoxy_putc(out_football_win, lx, y, 'x');
        w_gotoxy_putc(out_football_win, rx, y, 'x');
    }
    wattron(out_football_win, COLOR_PAIR(7));
    move(court.height - 1, court.width - 1);
    wrefresh(football_win);
    return ;
}

void redraw_score() {
    char red_score = score.red + '0'; 
    char blue_score = score.blue + '0'; 
    wattron(score_win, COLOR_PAIR(1));
    w_gotoxy_putc(score_win, 7, 3, red_score);
    wattron(score_win, COLOR_PAIR(7));
    w_gotoxy_putc(score_win, 9, 3, ':');
    wattron(score_win, COLOR_PAIR(4));
    w_gotoxy_putc(score_win, 11, 3, blue_score);
    wattron(score_win, COLOR_PAIR(7));
    return ;
}

void redraw_ball() {
    if (ball_status.v.x || ball_status.v.y) {
        char temp[512] = {0}; 
        // t = v / a; 表示球还有多少时间停止
        double time_out = sqrt(pow(ball_status.v.x, 2) + pow(ball_status.v.y, 2)) / sqrt(pow(ball_status.a.x, 2) + pow(ball_status.a.y, 2));
        sprintf(temp, "time out = %lf, vx = %lf, vy = %lf, ax = %lf, ay = %lf\n", time_out, ball_status.v.x, ball_status.v.y, ball_status.a.x, ball_status.a.y);
        Show_Message( , NULL, temp, 1);
        double t = 100000.0 / 1000000.0;
        if (t >= time_out) {
            //球运动的时间小于间隔时间
            Show_Message( , NULL, temp, 1);
            Show_Message( , NULL, "time out", 1);
            //此时球已经停止
            bzero(&ball_status.v, sizeof(ball_status.v)); 
            bzero(&ball_status.a, sizeof(ball_status.a)); 
        } else {
            // S = v0 * t + 1/2 * a * t^2 
            // 更新球的速度和位置
            ball.x += ball_status.v.x * t + ball_status.a.x * pow(t, 2) / 2;
            ball_status.v.x += ball_status.a.x * t;
            ball.y += (ball_status.v.y * t + ball_status.a.y * pow(t, 2) / 2) / 2;
            ball_status.v.y += ball_status.a.y * t;
            if (ball.x >= court.width || ball.x <= 0 || ball.y >= court.height || ball.y <= 0) {
                if (ball.x >= court.width - 1) {
                    if (ball.y >= court.height / 2 - 3 && ball.y <= court.height / 2 + 3) {
                        //球进了蓝队的球门(右边)
                        score.red++; //红队加一分
                        ball.x = court.width - 3;
                        ball.y = court.height / 2;
                        struct FootballMsg msg;
                        msg.type = FT_WALL;
                        sprintf(msg.msg, "%s of %s team, got 1 score!\n", ball_status.name, ball_status.by_team ? "blue" : "red");
                        Show_Message( , NULL, msg.msg, 1);
                        send_all(&msg);
                    }
                    ball.x = court.width - 1;
                } else if (ball.x <= 0) {
                    if (ball.y >= court.height / 2 - 3 && ball.y <= court.height / 2 + 3) {
                        score.blue++; //蓝队加一分
                        ball.x = 2;
                        ball.y = court.height / 2;
                        struct FootballMsg msg;
                        msg.type = FT_WALL;
                        sprintf(msg.msg, "%s of %s team, got 1 score!\n", ball_status.name, ball_status.by_team ? "blue" : "red");
                        Show_Message( , NULL, msg.msg, 1);
                        send_all(&msg);
                    }
                    ball.x = 0;
                }
                if (ball.y >= court.height) ball.y = court.height - 1;
                if (ball.y <= 0) ball.y = 0;
                bzero(&ball_status.v, sizeof(ball_status.v)); 
                bzero(&ball_status.a, sizeof(ball_status.a)); 
            }
        }
    } 
    w_gotoxy_putc(football_win, (int)ball.x, (int)ball.y, 'o');
    if (ball_status.by_team) {
        wattron(football_win, COLOR_PAIR(4));
    } else {
        wattron(football_win, COLOR_PAIR(1));
    }
    //w_gotoxy_putc(football_win, (int)ball.x + 1, (int)ball.y + 1, ACS_DEGREE);
    wattron(football_win, COLOR_PAIR(7));
    return ;
}

void redraw(int signum) {
    //全部重绘
    werase(out_football_win);
    //wclear(out_football_win);
    box(football_win, 0, 0);
    box(out_football_win, 0, 0);
    redraw_ball();
    redraw_score();
    redraw_team(redTeam);
    redraw_team(blueTeam);
    redraw_gate();
    //将计算结果发给客户端
    wrefresh(out_football_win);
    return ;
}
