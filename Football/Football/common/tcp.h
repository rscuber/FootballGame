/*************************************************************************
	> File Name: tcp.h
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Tue 11 Oct 2022 08:22:08 PM CST
 ************************************************************************/

#ifndef _TCP_H
#define _TCP_H

int socket_create_tcp(int port);
int socket_connect_tcp(const char *ip, int port);

#endif
