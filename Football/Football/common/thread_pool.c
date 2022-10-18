/*************************************************************************
	> File Name: thread_pool.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Thu 13 Oct 2022 07:25:06 PM CST
 ************************************************************************/

#include "head.h"

extern int red_reactor, blue_reactor;
extern struct Bpoint ball; //球的位置
extern struct BallStatus ball_status; //球的状态

void do_echo(struct User *user) {
    struct FootballMsg msg; 
    bzero(&msg, sizeof(msg));
    int ret = recv(user->fd, (void *)&msg, sizeof(msg), 0);
    if (ret != sizeof(msg)) {
        DBG(L_RED "Receive %d bytes data.\n" NONE, ret); 
        return ;
    }
    user->lives = 10;
    if (msg.type & FT_ACK) { //处理客户端的心跳ACK
        user->lives++; //客户端存活时间加一
        show_data_stream('n');
        if (user->team) {
            DBG(L_BLUE "%s -> " NONE "💗\n", user->name);
        } else {
            DBG(L_RED "%s -> " NONE "💗\n", user->name);
        }        
    } else if (msg.type & (FT_WALL | FT_MSG)) {//处理球员和系统消息
        if (user->team) {
            DBG(L_BLUE "%s: %s\n" NONE, user->name, msg.msg);
        } else {
            DBG(L_RED "%s: %s\n" NONE, user->name, msg.msg);
        } 
        strcpy(msg.name, user->name);
        msg.team = user->team;
        Show_Message( , user, msg.msg, 0);
        send_all(&msg);
        //send(user->fd, (void *)&msg, sizeof(msg), 0);
    } else if (msg.type & FT_FIN) { //处理用户退出游戏
        show_data_stream('n');
        if (user->team) {
            DBG(L_BLUE "%s " NONE L_PINK "logout!\n" NONE, user->name);
        } else {
            DBG(L_RED "%s " NONE L_PINK "logout!\n" NONE, user->name);
        } 
        char temp[128] = {0};
        sprintf(temp, "%s logout.", user->name);
        Show_Message( , NULL, temp, 1);
        user->isOnline = 0;
        //w_gotoxy_putc(out_football_win, user->loc.x, user->loc.y, ' ');
        int sub_epollfd = user->team ? blue_reactor : red_reactor;
        del_events(sub_epollfd, user->fd); //清理已退出用户
    } else if (msg.type & FT_CTL) { //处理用户动作请求
        char temp[128] = {0};
        sprintf(temp, "Ctrl Message kick = %d.", msg.ctl.strength);
        Show_Message( , user, temp, 0); 
        memset(temp, 0, sizeof(temp));
        //对用户的位置信息有恶意竞争的隐患
        if (msg.ctl.action & ACTION_DFT) {
            //if (msg.ctl.dirx || msg.ctl.diry) {
            show_data_stream('n');
            user->loc.x += msg.ctl.dirx;
            user->loc.y += msg.ctl.diry;
            //注意出界问题
            if (user->loc.x <= 1) user->loc.x = 1;
            if (user->loc.x >= court.width + 2) user->loc.x = court.width + 2;
            if (user->loc.y <= 0) user->loc.y = 0;
            if (user->loc.y >= court.height + 1) user->loc.y = court.height + 1;
        } else if (msg.ctl.action & ACTION_KICK) {
            show_data_stream('k');
            sprintf(temp, "bx = %lf, by = %lf, px = %d, py = %d", ball.x, ball.y, user->loc.x, user->loc.y);
            Show_Message( , user, temp, 0); //显示人与球的位置坐标
            if (can_kick(&user->loc, msg.ctl.strength)) {
                //处理踢球动作
                memset(temp, 0, sizeof(temp));
                ball_status.by_team = user->team; 
                strcpy(ball_status.name, user->name);
                sprintf(temp, "vx = %lf, vy = %lf, ax = %lf, ay = %lf", ball_status.v.x, ball_status.v.y, ball_status.a.x, ball_status.a.y);
                Show_Message( , user, temp, 0); 
            }
        } else if (msg.ctl.action & ACTION_STOP) {
            show_data_stream('s');
            if (can_access(&user->loc)) { 
                //若能接触球
                bzero(&ball_status.v, sizeof(ball_status.v));
                bzero(&ball_status.a, sizeof(ball_status.a));
                sprintf(temp, "Stop the football.");
                Show_Message( , user, temp, 0);
            } 
        } else if (msg.ctl.action & ACTION_CARRY) {
            show_data_stream('c');
            sprintf(temp, "Try carry the football."); 
            Show_Message( , user, temp, 0);
            //球与球员暂时绑定,别人如何抢球
        }
    }
    return ;
}

void task_queue_init(struct task_queue *taskQueue, int size, int epollfd) {
    taskQueue->size = size;  
    taskQueue->epollfd = epollfd;
    taskQueue->team = calloc(size, sizeof(void *));
    taskQueue->head = taskQueue->tail = taskQueue->total = 0;
    pthread_mutex_init(&taskQueue->mutex, NULL);
    pthread_cond_init(&taskQueue->cond, NULL);
    return ;
}

int task_queue_empty(struct task_queue *taskQueue) {
    return taskQueue->total == 0;
}

int task_queue_full(struct task_queue *taskQueue) {
    return taskQueue->total == taskQueue->size;
}

void task_queue_push(struct task_queue *taskQueue, struct User *user) {
    pthread_mutex_lock(&taskQueue->mutex); 
    if (task_queue_full(taskQueue)) {
        DBG(L_RED "<Full> " NONE "The task queue is full!\n"); 
    }
    taskQueue->team[taskQueue->tail++] = user;
    taskQueue->tail %= taskQueue->size;
    taskQueue->total += 1;
    DBG(L_YELLOW "<Push> " NONE "%s\n", user->name);
    pthread_cond_signal(&taskQueue->cond);
    pthread_mutex_unlock(&taskQueue->mutex);
    return ;
}

struct User *task_queue_pop(struct task_queue *taskQueue) {
    pthread_mutex_lock(&taskQueue->mutex);    
    while (task_queue_empty(taskQueue)) {
        DBG(L_YELLOW "<Empty> " NONE "waiting for task...\n");
        pthread_cond_wait(&taskQueue->cond, &taskQueue->mutex); 
    }
    struct User *user = taskQueue->team[taskQueue->head++]; 
    taskQueue->head %= taskQueue->size;
    taskQueue->total -= 1;
    DBG(L_YELLOW "<Pop> " NONE "%s\n", user->name);
    pthread_mutex_unlock(&taskQueue->mutex);    
    return user;
}

void *thread_run(void *arg) {
    pthread_detach(pthread_self()); 
    struct task_queue *taskQueue = (struct task_queue *)arg;
    while (1) {
        struct User *user = task_queue_pop(taskQueue); 
        do_echo(user);
    }
    return NULL;
}
