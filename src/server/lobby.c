#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "gamestructs.h"
#include "socket.h"
#include "server_comms.h"
#include "comms.h"
#include <stdlib.h>
#include <unistd.h>
#include "protocol.h"
#include <string.h>

#define LOBBY_SIZE 5
extern Player players[LOBBY_SIZE];

void start_lobby(int listenfd) {
    fd_set fds;
    fd_set read_fds;
    FD_ZERO(&fds);
    FD_SET(listenfd, &fds);
    int max_fd = listenfd;

    for (int i = 0; i < LOBBY_SIZE; i++) {
        players[i].fd = -1;
        players[i].state = DISCONNECTED;
        memset(players[i].name, '\0', 32);
    }

    int started = 0;
    int joined = 0;

    while (!started) {
        read_fds = fds;
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            continue;
        }

        if (FD_ISSET(listenfd, &read_fds)) {
            int fd_n = accept_connection(listenfd);
            if (fd_n != -1) {
                FD_SET(fd_n, &fds);
                // TODO: we really gotta go thru and just remove all these expansions into ternaries even if kai has an aneurysm
                if (fd_n > max_fd) {
                    max_fd = fd_n;
                }
                for (int i = 0; i < LOBBY_SIZE; i++) {
                    if (players[i].state == DISCONNECTED) {
                        players[i].fd = fd_n;
                        players[i].inbuf = 0;
                        players[i].ready = 0;
                        players[i].p_id = i+1;
                        players[i].state = PENDING;
                        sprintf(players[i].name, "%d", i); // we gotta figure out how the join packet works but for now im just putting an int.
                        printf("%s joined successfully\n", players[i].name);
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < LOBBY_SIZE; i++) {
            if (players[i].state != DISCONNECTED && FD_ISSET(players[i].fd, &read_fds)) {
                response ret = s_read(&players[i]);
                sleep(1);
                printf("%d", ret);
                if (ret == READ_SUCCESS && players[i].ready) {
                    // printf("here\n");
                    Packet *pkt = &players[i].active;
                    if (players[i].state == PENDING) {
                        if (pkt->type == P_JOIN) {
                            strncpy(players[i].name, ptos(pkt), 31);
                            players[i].name[32] = '\0';
                            //players[i].state = READY;
                            joined++;
                            printf("%s joined properly\n", players[i].name);
                        } else if (pkt->type == P_START) {
                            // at least for testing im giving my player the ability to start the game.
                            printf("success start game\n");
                            started = 1;
                            return;
                        } else {
                            printf("waiting for correct packet\n");
                        }
                    }
                    players[i].ready = 0;
                } else if (ret == -2) {
                    printf("player %s disconnected", players[i].name);
                    close(players[i].fd);
                // TODO: finish this
                }
            }
        }
    }
    printf("woohoo\n");
}

