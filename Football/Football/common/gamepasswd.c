/*************************************************************************
	> File Name: gamepasswd.c
	> Author: 宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Mon 10 Oct 2022 03:31:32 PM CST
 ************************************************************************/

#include "head.h"

extern struct user_passwd *userPasswd;
extern const int max_users;

int read_passwd(const char *passwdFile, struct user_passwd *userPasswd, int len) {
    //将文件的用户名和对应的密码读到内存中
    FILE *fp;
    if ((fp = fopen(passwdFile, "r")) == NULL) {
        return -1;
    } 
    memset(userPasswd, 0, sizeof(struct user_passwd) * len);
    char buf[128] = {0};
    int ind = 0;
    while (fgets(buf, 128, fp) != NULL) {
        //username:passwd
        size_t size = strchr(buf, ':') - buf;
        strncpy(userPasswd[ind].username, buf, size);
        strncpy(userPasswd[ind].passwd, buf + size + 1, strlen(buf) - size - 2);
        DBG(L_YELLOW "strlen(username) = %ld, strlen(passwd) = %ld\n" NONE, strlen(userPasswd[ind].username), strlen(userPasswd[ind].passwd));
        userPasswd[ind].flag = 1; //标记已用
        ind += 1;
    }
    fclose(fp);
    return 0;
}

int write_passwd(const char *passwdFile, struct user_passwd *userPasswd, int len) {
    FILE *fp; 
    if ((fp = fopen(passwdFile, "w+")) == NULL) {
        return -1; 
    }
    for (int i = 0; i < len; i++) {
        if (!userPasswd[i].flag) break;
        fprintf(fp, "%s:%s\n", userPasswd[i].username, userPasswd[i].passwd);
    }
    fclose(fp);
    return 0;
}

int check_passwd(struct user_passwd *userPasswd, int len, const char *username, const char *passwd) {
    for (int i = 0; i < len; i++) {
        if (!userPasswd[i].flag) break;
        DBG(L_YELLOW "userPasswd[%d]: flag = %d, username = %s, passwd = %s\n" NONE, i, userPasswd[i].flag, userPasswd[i].username, userPasswd[i].passwd);
        if (!strcmp(username, userPasswd[i].username) && !strcmp(passwd, userPasswd[i].passwd)) {
            DBG(L_BLUE "Check password true.\n" NONE);
            return 0;
        }
    }
    DBG(L_RED "Check password false.\n" NONE);
    return -1;
}


int check_user(struct user_passwd *userPasswd, int len, struct LogRequest *request) {
    return check_passwd(userPasswd, len, request->name, request->passwd);
}

int change_passwd(const char *passwdFile, struct user_passwd *userPasswd, int len, const char *username, char *newpasswd) {
    FILE *fp = fopen(passwdFile, "w+");
    if (fp == NULL) return -1;
    for (int i = 0; i < len; i++) {
        if (!userPasswd[i].flag) continue;
        if (!strcmp(username, userPasswd[i].username)) {
            strcpy(userPasswd[i].passwd, newpasswd);
            write_passwd(passwdFile, userPasswd, len);
            DBG(L_BLUE "Change password successfully.\n" NONE);
            fclose(fp);
            return 0;
        }
    }
    fclose(fp);
    DBG(L_RED "Change password fail.\n" NONE);
    return -1;
}
