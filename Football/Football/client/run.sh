#!/bin/bash
gcc client.c ../common/common.c ../common/udp_client.c ../common/gamepasswd.c ../common/epoll_ctl_event.c ../common/client_recv.c ../common/send_chat.c ../common/send_ctl.c  ../common/show_strength.c ../common/show_data_stream.c ../common/game_ui.c -lpthread -lncurses
if [[ $? -eq 0 ]];then
    ./a.out
fi
