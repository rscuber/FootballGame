/*************************************************************************
	> File Name: file_transfer.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Sun 09 Oct 2022 08:56:21 PM CST
 ************************************************************************/

#include "head.h"

int recv_file_from_socket(int sockfd, char *name, char *dir) {
    char path[1024] = {0};    
    sprintf(path, "%s/%s", dir, name);
    int fd = open(path, O_CREAT | O_WRONLY, 0644);
    if (fd < 0) {
        close(fd);
        return -1;
    }
    while (1) {
        char buf[512] = {0};
        int rsize = recv(sockfd, buf, sizeof(buf), 0);
        if (rsize <= 0) {
            close(fd);
            return rsize;
        }
        int nwrite = write(fd, buf, rsize);
        if (nwrite != rsize) {
            close(fd);
            return -1;
        }
    }
    close(fd);
    return 0;
}

int send_file_to_socket(int confd, char *name) {
    int fd;
    if ((fd = open(name, O_RDONLY)) < 0) {
        return -1;
    }
    while (1) {
        char buf[512] = {0};
        int rsize = read(fd, buf, sizeof(buf));
        if (rsize <= 0) {
            close(fd);
            close(confd);
            DBG(L_YELLOW "<DEBUG> " NONE "Send file end.\n");
            return rsize;
        }
        send(confd, buf, rsize, 0); 
    }
    return 0;
}
