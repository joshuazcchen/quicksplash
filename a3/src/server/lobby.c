#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "gamestructs.h"
#include "socket.h"
#include "comms.h"
#include <stdlib.h>
#include <unistd.h>

#define LOBBY_SIZE 5 // TODO: move to header 

Player players[LOBBY_SIZE];

int game_read(Player *p); // TODO: move to header

void start_lobby(int listenfd) {
    fd_set fds;
    fd_set read_fds;
    FD_ZERO(&fds);
    FD_SET(listenfd, &fds);
    int max_fd = listenfd;

    for (int i = 0; i < LOBBY_SIZE; i++) {
        players[i].fd = -1;
    }

    while (1) {
        read_fds = fds;
        // TODO: error check this
        select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        // manages new connection attempts
        if (FD_ISSET(listenfd, &read_fds)) {
            int fd_n = accept_connection(listenfd); // TODO: fix my stupid variable naming, fd_n is the nth fd, but breaks convention of fds, which in itself is kinda a stupid name
            if (fd_n != -1) {
                FD_SET(fd_n, &fds);
                if (fd_n > max_fd) {
                    max_fd = fd_n;
                }
                for (int i = 0; i < LOBBY_SIZE; i++) {
                    if (players[i].fd == -1) {
                        players[i].fd = fd_n;
                        players[i].inbuf = 0;
                        break;
                    }
                }
            }
        }

        // listen for packets
        for (int i = 0; i < LOBBY_SIZE; i++) {
            if (players[i].fd != -1 && FD_ISSET(players[i].fd, &read_fds)) {
                if (game_read(&players[i]) == 1) {
                    printf("test buffer success, buffer read from %d of type %d %s\n", players[i].fd, players[i].partial.type, players[i].partial.data);
                } else {
                    printf("test buffer failed or not finished %d", players[i].fd);
                    close(players[i].fd);
                    FD_CLR(players[i].fd, &fds);
                    players[i].fd = -1;
                }
            }
        }
    }
}
