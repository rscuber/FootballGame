/*************************************************************************
	> File Name: head.h
	> Author:  宫局仁的供橘人
	> Motto: Barefoot makes wheels
	> Created Time: Sun 02 Oct 2022 10:39:54 AM CST
 ************************************************************************/

#ifndef _HEAD_H
#define _HEAD_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <ncurses.h>
#include <locale.h>
#include "datatype.h"
#include "game_ui.h"
#include "color.h"
#include "gamepasswd.h"
#include "epoll_ctl_event.h"
#include "udp_server.h"
#include "udp_client.h"
//#include "global.h"
#include "math.h"
#include "common.h"
#include "thread_pool.h"
#include "sub_reactor.h"
#include "heart_beat.h"
#include "client_recv.h"
#include "server_exit.h"
#include "send_chat.h"
#include "send_ctl.h"
#include "redraw.h"
#include "ball_status.h"
#include "show_strength.h"
#include "send_all.h"
#include "show_data_stream.h"

#ifdef DEBUG
#define DBG(fmt, args...) {\
    printf(L_GREEN "[%s : %s : %d] " NONE, __FILE__, __func__, __LINE__);\
    printf(fmt, ##args);\
    printf("\n");\
}
#else
#define DBG(fmt, arg...)
#endif

#ifdef DEBUG2
#define DBG2(fmt, args...) {\
    printf(L_GREEN "[%s : %s : %d] " NONE, __FILE__, __func__, __LINE__);\
    printf(fmt, ##args);\
    printf("\n");\
}
#else
#define DBG2(fmt, arg...)
#endif

#define handle_error(msg) {\
    perror(msg);\
    exit(EXIT_FAILURE);\
}

#endif
