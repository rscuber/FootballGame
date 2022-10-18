/*************************************************************************
	> File Name: udp_server.h
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Tue 11 Oct 2022 08:23:45 PM CST
 ************************************************************************/

#ifndef _UDP_SERVER_H
#define _UDP_SERVER_H

int socket_create_udp_server(int port);
int udp_connect_client(int epollfd, struct sockaddr_in *addr);
int udp_accept(int epollfd, int listener, struct User *user);
void add_to_sub_reactor(int sub_epollfd, struct User *user);

#endif
