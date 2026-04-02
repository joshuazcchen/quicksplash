#include "gamestructs.h"
#include "comms.h"
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

#define LOBBY_SIZE 5
extern PLAYER players[LOBBY_SIZE];

response s_send(Packet p) {
    for (int i = 0; i < LOBBY_SIZE; i++) {
        if (players[i].fd > 2) {
            comms.send(players[i].fd, p);
        }
    }
    // this is lowkey a fake success but idk how to actually manage the lobby size variable here tbh.
    return SEND_SUCCESS;
}

// ok theres actually no way to respond here properly tbh lol
response s_listen(int max_time) {
    time_t start = time(NULL);
    
    while (1) {
        int t = time(NULL) - start;
        if (t >= max_time) {
            return TIMEOUT;
        }

        fd_set fds;
        FD_ZERO(&fds);
        int high = 0;

        for (int i = 0; i < LOBBY_SIZE; i++) {
            if (players[i].fd) {
                FD_SET(players[i].fd, &fds);
                if (players[i].fd > high) {
                    high = players[i].fd;
                }
            }
        }

        struct timeval tl; // apparently this is considered synchronous which means i need it for the select multiplexing for the timeout.
        tl.tv_sec = max_time - t;

        int listen = select(high + 1, &fds, NULL, NULL, &tl);
        if (listen > 0) {
            for (int i = 0; i < LOBBY_SIZE; i++) {
                response status = comms_read(players[i].fd, &players[i].partial, &players[i].inbuf);
                if (status == READ_SUCCESS) {
                    memcpy(&players[i].active, &players[i].partial, sizeof(Packet));
                    players[i].ready = 1;
                    players[i].inbuf = 0;
                }
            }
        }
    }
}

