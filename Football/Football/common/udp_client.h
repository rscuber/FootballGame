/*************************************************************************
	> File Name: udp_client.h
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Thu 13 Oct 2022 03:37:40 PM CST
 ************************************************************************/

#ifndef _UDP_CLIENT_H
#define _UDP_CLIENT_H

int socket_create_udp_client();
int socket_connect_udp_game(const char *ip, int port);

#endif
