#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "gamestructs.h"
#include "socket.h"
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
                    if (players[i].fd == -1) {
                        players[i].fd = fd_n;
                        players[i].inbuf = 0;
                        players[i].ready = 0;
                        sprintf(players[i].name, "%d", i); // we gotta figure out how the join packet works but for now im just putting an int.
                        printf("player joined successfully\n");
                        joined++;
                        break;
                    }
                }
                if (joined >= LOBBY_SIZE) {
                    printf("lobby full");
                    started = 1;
                }
            }
        }
    }
    printf("woohoo");
}
