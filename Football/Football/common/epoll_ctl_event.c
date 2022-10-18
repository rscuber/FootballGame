/*************************************************************************
	> File Name: epoll_ctl_event.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Tue 11 Oct 2022 11:09:05 AM CST
 ************************************************************************/

#include "head.h"

int add_events(int epollfd, int fd, int events) {
    struct epoll_event ev; 
    ev.data.fd = fd;
    ev.events = events;
    return epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

int add_events_ptr(int epollfd, int fd, int events, struct User *user) {
    struct epoll_event ev; 
    ev.data.ptr = (void *)user;
    ev.events = events;
    return epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

int del_events(int epollfd, int fd) {
    return epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
}

int del_events_ptr(int epollfd, int fd, struct User *user) {
    return epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
}
