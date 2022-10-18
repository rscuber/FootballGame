/*************************************************************************
	> File Name: epoll_ctl_event.h
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Tue 11 Oct 2022 11:12:21 AM CST
 ************************************************************************/

#ifndef _EPOLL_CTL_EVENT_H
#define _EPOLL_CTL_EVENT_H

int add_events(int epollfd, int fd, int events);
int add_events_ptr(int epollfd, int fd, int events, struct User *user);
int del_events(int epollfd, int fd);
int del_events_ptr(int epollfd, int fd, struct User *user);

#endif
