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
		players[i].c_state = HEADER;
		players[i].h_inbuf = 0;
		players[i].p_inbuf = 0;
		players[i].ready = 0;
		players[i].active.data = NULL;
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
                if (fd_n > max_fd) max_fd = fd_n;
                for (int i = 0; i < LOBBY_SIZE; i++) {
                    if (players[i].state == DISCONNECTED) {
                        players[i].fd = fd_n;
                        players[i].p_id = i+1; 
                        players[i].state = PENDING;
						players[i].c_state = HEADER;
						players[i].h_inbuf = 0;
						players[i].p_inbuf = 0;
						players[i].ready = 0;
						// hi im not sure why i split these its just that having a random green in the middle
						// was bugging me a bit lol
                        sprintf(players[i].name, "%d", i); 
						printf("Connection received from %d\n", players[i].fd);
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < LOBBY_SIZE; i++) {
            if (players[i].state != DISCONNECTED && FD_ISSET(players[i].fd, &read_fds)) {
                response ret = s_read(&players[i]);

                if (ret == READ_SUCCESS && players[i].ready) {
                    Packet *pkt = &players[i].active;
                    if (players[i].state == PENDING) {
                        if (pkt->header.type == PKT_JOIN) {
                            strncpy(players[i].name, ptos(pkt), 31);
							// not fully remembering how i did it before but 32 would null ptr error so were gonna not.
							// swapped this to 31 instead of 32
                            players[i].name[31] = '\0';
							//players[i].state = READY;
							joined++;
							players[i].state = READY;
							printf("%s joined properly\n", players[i].name);
						} 
					} else if (pkt->header.type == PKT_START) {
						printf("success!!!!!\n");
						// at least for testing im giving my player the ability to start the game.
						// // TODO TODO: ADD VERIFICATION THAT ITS THE ACTUAL DEDICATED HOST CLIENT
						printf("success start game\n");
						started = 1;
						free(pkt->data);
						pkt->data = NULL;
						return;
					}
					free(pkt->data);
					pkt->data = NULL;
					players[i].ready = 0;
				} else if (ret == CLIENT_DISCONNECT) {
					close(players[i].fd);
					players[i].state = DISCONNECTED;
					players[i].fd = -1;
					if (players[i].active.data) {
						free(players[i].active.data);
						players[i].active.data = NULL;
						printf("Cleared data from disconnected player\n");
					}
					printf("Player disconnected\n");
					// TODO: finish this
				}
			}
		}
	}
//	printf("woohoo\n");
}
