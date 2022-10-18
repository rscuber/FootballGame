/*************************************************************************
	> File Name: datatype.h
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Tue 11 Oct 2022 10:49:33 PM CST
 ************************************************************************/

#ifndef _DATATYPE_H
#define _DATATYPE_H

#define MAX_EVENTS 10
#define MAX_TEAM_USERS 50
#define MAX_MSG 1024

#define SIGNUP 0x01
#define SIGNIN 0x02
//#define SIGNOUT 0x04

#define SIGNIN_SUCCESS 0x04
#define SIGNIN_FAIL 0x08

#define FT_HEART 0x10   //服务端发送的心跳检测
#define FT_WALL 0x20    //服务端广播的游戏消息
#define FT_MSG 0x40     //客户端发送的信息，服务端转发的信息
#define FT_ACK 0x80     //客户端对服务端心跳检测的确认
#define FT_FIN 0x0100   //客户端或服务端下线时对彼此的告知
#define FT_CTL 0x0200   //客户端发送的控制信息(json发送)
#define FT_GAME 0x0400  //服务端向客户端广播的游戏实时画面(json发送)
#define FT_SCORE 0x0800 //服务端向客户端广播的游戏比分变化(json发送，包括进球人)
#define FT_GAMEOVER 0x1000 //游戏结束
#define FT_MAP 0x2000

struct Score {
    int red;
    int blue;
};

struct Bpoint {
    double x;
    double y;
};

struct Speed {
    double x;
    double y;
};

struct Aspeed {
    double x;
    double y;
};

struct BallStatus {
    struct Speed v; //速度
    struct Aspeed a; //加速度
    int by_team; //which team
    char name[20]; //被谁控制
};

struct LogRequest {
    int type;
    char name[20];
    char passwd[20];
    int team; // 0:red 1:blue
    char msg[512];
};

struct LogResponse {
    int type; 
    char msg[512];
};

struct Point {
    int x, y;
};

struct User {
    int fd; //每个用户对应一个socket
    int team; // 0:red  1:blue
    int score;
    char name[20];
    char msg[512];
    int isOnline;
    int lives; //存活时间(次数)
    struct Point loc;
};

struct Map {
    int width;
    int height;
    struct Point start;
    int gate_width;
    int gate_height;
};

#define ACTION_KICK 0x01  //踢球
#define ACTION_CARRY 0x02 //带球
#define ACTION_STOP 0x04 //停球
#define ACTION_DFT 0x08  //普通移动动作

struct Ctrl {
    int dirx, diry; //方向移动步数
    int strength; //力度
    int action; //动作
};

//登录后唯一发送的数据包
struct FootballMsg {
    int type;
    int size;
    int team;
    char name[20];
    struct Ctrl ctl;
    char msg[MAX_MSG];
};

#endif
