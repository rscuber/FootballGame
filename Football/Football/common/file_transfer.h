/*************************************************************************
	> File Name: file_transfer.h
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Sun 09 Oct 2022 08:55:20 PM CST
 ************************************************************************/

#ifndef _FILE_TRANSFER_H
#define _FILE_TRANSFER_H

int recv_file_from_socket(int sockfd, char *name, char *dir);
int send_file_to_socket(int confd, char *name);

#endif
