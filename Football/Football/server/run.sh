#!/bin/bash
gcc server.c ../common/common.c  ../common/gamepasswd.c ../common/epoll_ctl_event.c ../common/udp_server.c ../common/thread_pool.c ../common/sub_reactor.c ../common/heart_beat.c ../common/server_exit.c ../common/redraw.c ../common/show_data_stream.c  ../common/send_all.c ../common/ball_status.c ../common/game_ui.c -lpthread -lncurses -lm
if [[ $? -eq 0 ]];then
    ./a.out
fi
