/*************************************************************************
	> File Name: common.h
	> Author: GodOfRang
	> Motto: Barefoot makes wheels
	> Created Time: Sun 02 Oct 2022 10:33:07 PM CST
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

char conf_value[512];
int make_nonblock(int fd);
int make_block(int fd);
char *get_conf_value(const char *file, const char *key);

#endif
