/*************************************************************************
	> File Name: common.c
	> Author: GodOfRang
	> Motto: Barefoot makes wheels
	> Created Time: Sun 02 Oct 2022 10:32:55 PM CST
 ************************************************************************/

#include "head.h"

int make_nonblock(int fd) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL)) < 0) {
        return -1;
    }
    flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags);
}

int make_block(int fd) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL)) < 0) {
        return -1;
    }
    flags &= ~O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags);
}

char *get_conf_value(const char *file, const char *key) {
    FILE *fp;
    if ((fp = fopen(file, "r")) == NULL) {
        return NULL;
    }
    char *line = NULL, *sub = NULL;
    size_t len = 0, nread = 0;
    bzero(conf_value, sizeof(conf_value));
    while ((nread = getline(&line, &len, fp)) != -1) {
        if ((sub = strstr(line, key)) == NULL) continue;
        if (sub == line && line[strlen(key)] == '=') {
            strcpy(conf_value, line + strlen(key) + 1);
            int tail = strlen(conf_value) - 1;
            //去掉最后的换行符
            if (conf_value[tail] == '\n') conf_value[tail] = '\0';
        }
    }
    free(line);
    fclose(fp);
    if (!strlen(conf_value)) return NULL;
    return conf_value;
}

