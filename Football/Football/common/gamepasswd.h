/*************************************************************************
	> File Name: gamepasswd.h
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Mon 10 Oct 2022 11:37:41 AM CST
 ************************************************************************/

#ifndef _GAMEPASSWD_H
#define _GAMEPASSWD_H

struct user_passwd {
    char username[20];
    char passwd[20];
    int flag;  // 0:未用 1:已用
};

int read_passwd(const char *passwdFile, struct user_passwd *userPasswd, int len);
int write_passwd(const char *passwdFile, struct user_passwd *userPasswd, int len);
int check_passwd(struct user_passwd *userPasswd, int len, const char *username, const char *passwd);
int check_user(struct user_passwd *userPasswd, int len, struct LogRequest *request);
int change_passwd(const char *passwdFile, struct user_passwd *userPasswd, int len, const char *username, char *newpasswd);

#endif
