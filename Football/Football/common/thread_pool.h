/*************************************************************************
	> File Name: thread_pool.h
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Thu 13 Oct 2022 07:25:03 PM CST
 ************************************************************************/

#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#define MAX_TASKS 100
#define MAX_THREADS 10

struct task_queue {
    int head, tail, size, total, epollfd;
    struct User **team;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

void do_echo(struct User *user);
void task_queue_init(struct task_queue *taskQueue, int size, int epollfd);
int task_queue_empty();
int task_queue_full();
void task_queue_push(struct task_queue *taskQueue, struct User *user);
struct User *task_queue_pop(struct task_queue *taskQueue);
void *thread_run(void *arg);

#endif
