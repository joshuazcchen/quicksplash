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

extern Player players[LOBBY_SIZE];
int PLR_COUNT;

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
						printf("\033[1;35m[ SERVER ]\033[0m \033[1;32mlobby.c:\033[0m Connection received from \033[1;33m%d\033[0m.\n", players[i].fd);
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
							PLR_COUNT++;
							players[i].state = READY;
							printf("\033[1;35m[ SERVER ]\033[0m \033[1;32mlobby.c:\033[0m Received join packet from \033[1;33m%d\033[0m, user added as \033[1;33m%s\033[0m.\n", players[i].p_id, players[i].name);
							Packet host_pkt;
							if (players[i].p_id == 1) {
								host_pkt = strtopkt(PKT_JOIN, "YOU ARE HOST NOW CONGRATULATIONS");
							} else {
								host_pkt = strtopkt(PKT_JOIN, "Orange");
							}
							s_slide(players[i].fd, &host_pkt); 
							free(host_pkt.data);
						} 
					} else if (pkt->header.type == PKT_START) {
						if (players[i].p_id == 1) {
							printf("\033[1;35m[ SERVER ]\033[0m \033[1;32mlobby.c:\033[0m Received start packet from host.\n");
							started = 1;
							char plrct[32];
							snprintf(plrct, 32, "%d", PLR_COUNT);
							printf("\033[1;35m[ SERVER ]\033[0m \033[1;32mlobby.c:\033[0m Starting with \033[1;33m%d player(s)\033[0m.\n", PLR_COUNT);
							Packet pktstart = strtopkt(PKT_START, plrct);
							s_send(&pktstart);
							free(pkt->data);
							free(pktstart.data);
							pkt->data = NULL;
							return;
						}
					}
					if (players[i].active.data != NULL) {
						free(players[i].active.data);
						players[i].active.data = NULL;
					}
				} else if (ret == CLIENT_DISCONNECT) {
					printf("\033[1;35m[ SERVER ]\033[0m \033[1;31mlobby.c:\033[0m Player %s disconnected from lobby.\n", players[i].name);
					close(players[i].fd);
					players[i].state = DISCONNECTED;
					players[i].fd = -1;
					PLR_COUNT--;
					if (players[i].active.data) {
						free(players[i].active.data);
						players[i].active.data = NULL;
					}
				}
			}
		}
	}
}
