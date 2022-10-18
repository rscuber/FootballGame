/*************************************************************************
	> File Name: sub_reactor.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Fri 14 Oct 2022 04:28:05 PM CST
 ************************************************************************/

#include "head.h"

void *sub_reactor(void *arg) {
    //任务队列+多线程=线程池
    struct task_queue *taskQueue = (struct task_queue *)arg; 
    pthread_t *tid = (pthread_t *)calloc(MAX_THREADS, sizeof(pthread_t));
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_create(&tid[i], NULL, thread_run, (void *)taskQueue);
    }
    struct epoll_event ev, events[MAX_EVENTS];
    //使epoll屏蔽时钟信号
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGALRM);
    for (;;) {
        DBG(L_YELLOW "<SubReactor> " NONE "Epoll waiting...\n");
        int nfds = epoll_pwait(taskQueue->epollfd, events, MAX_EVENTS, -1, &sigset); 
        if (nfds == -1) handle_error("epoll_wait");
        for (int i = 0; i < nfds; i++) {
            struct User *user = (struct User *)events[i].data.ptr;
            DBG(L_BLUE "%s has request.\n" NONE ,user->name);
            if (events[i].events & EPOLLIN) {
                task_queue_push(taskQueue, user); 
            }     
        }
    }
    return NULL;
}
