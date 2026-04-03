#include "gamestructs.h"
#include "comms.h"
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

#define LOBBY_SIZE 5
extern Player players[LOBBY_SIZE];

// sends a packet from the server to each of the clients.
// it is imperative that the client does not edit this. i dont think itll actually break anything, but it is probably just best convention
response s_send(Packet p) {
    for (int i = 0; i < LOBBY_SIZE; i++) {
        if (players[i].fd > 2) {
            comms_send(players[i].fd, p);
        }
    }
    // this is lowkey a fake success but idk how to actually manage the lobby size variable here tbh.
    return SEND_SUCCESS;
}

// ok theres actually no way to respond here properly tbh lol
response s_listen(int max_time) {
    time_t start = time(NULL);
    
    // repeat for max_time time until time out or everyone has answered.
    // in case of client disconnect, you will just have to wait out the max_time.
    // however we could probably make that work better by using signals or smth?
    while (1) {
        int t = time(NULL) - start;
        if (t >= max_time) {
            return TIMEOUT;
        }

        // initialize a set of file descriptors and zero them out
        fd_set fds;
        FD_ZERO(&fds);
        // this is the highest file descriptor, we could theoretically swap this to just directly go through the players but honestly, i think this is still best because it lets us work directly with said file descriptors and if someone disconnects, we can easily rearrange themw ithout further problem.
        // also it lets us basically stop extraneous checks.
        int high = 0;

        for (int i = 0; i < LOBBY_SIZE; i++) {
            if (players[i].fd) {
                FD_SET(players[i].fd, &fds);
                if (players[i].fd > high) {
                    high = players[i].fd;
                }
            }
        }

        // set up the struct that will be used to time out if max_time has passed
        struct timeval tl; // apparently this is considered synchronous which means i need it for the select multiplexing for the timeout.
        tl.tv_sec = max_time - t;

        int listen = select(high + 1, &fds, NULL, NULL, &tl);
        if (listen > 0) {
            for (int i = 0; i < LOBBY_SIZE; i++) {
                // try to read from each player, if it reads properly then great, this was the player we wanted to listen to, otherwise, keep goin'.
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

