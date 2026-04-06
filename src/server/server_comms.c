#include "gamestructs.h"
#include "comms.h"
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

extern Player players[LOBBY_SIZE];
extern int PLR_COUNT;

// sends a packet from the server to each of the clients.
// it is imperative that the client does not edit this. i dont think itll actually break anything, but it is probably just best convention
response s_send(Packet *p) {
    for (int i = 0; i < PLR_COUNT; i++) {
		// same check as before but i figure its good to comment:
		// this basically just verifies that its not trying to write to the host stdin/stdout/stderr
        if (players[i].fd > 2) {
            if (comms_send(players[i].fd, p) == SEND_FAIL) {
				return SEND_FAIL; // if one single send failed, this should return a failure...
			}
        }
    }
    // this is lowkey a fake success but idk how to actually manage the lobby size variable here tbh.
    return SEND_SUCCESS;
}

// basically 1:1 copy of the client side one im not gonna recomment it fully so just read teh comments from the client side.
// the only meaningful difference is just that the server takes extra care not to kill itself through failed stuff.
// The only condition where I actually exit as server is for if malloc fails despite a valid call because that would almost
// certainly imply that we have an actual true fail.
//
// also this is entirely serverside logic. IT IS SIMILAR BUT NOT THE SMAE AS THE CLIENT DONT MESS UP
response s_read(Player *p) {
    response ret;

	if (p->c_state == HEADER) {
		ret = comms_read(p->fd, &p->active.header, &p->h_inbuf, sizeof(PacketHeader));
		if (ret == READ_SUCCESS) {
			p->active.header.length = ntohs(p->active.header.length);
			printf("\033[1;35m[ SERVER ]\033[0m \033[1;33mserver_comms.c:\033[0m Server awaiting for packet of type \033[1;33m%d\033[0m for \033[1;33m%d bytes\033[0m\n", p->active.header.type, p->active.header.length); 
			p->c_state = PAYLOAD;

			if (p->active.header.length > 0) {
				p->active.data = malloc(p->active.header.length);
				if (!p->active.data) {
					fprintf(stderr, "\033[1;35m[ SERVER ]\033[0m \033[1;31mserver_comms.c:\033[0m Server side fatal error in malloc, likely out of memory, printing error log and terminating:\n\t");
					perror("malloc");
					exit(1); // server should never actually exit but if something breaks in the malloc then we break. 
							 // this just prevents a segfault later because theres no way around it later on.
				}
			} else {
				// same break as before
				if (p->active.header.length == 0) {
					p->active.data = NULL;
					p->h_inbuf = 0;
					p->ready = 1;
					return READ_SUCCESS;
				} else {
					printf("\033[1;35m[ SERVER ]\033[0m \033[1;31mserver_comms.c:\033[0m Did not receive proper header from Player \033[1;33m%s\033[0m.\n", p->name);
				}
			}
		} else { 
			return ret;
		}
	}

	if (p->c_state == PAYLOAD) {
		ret = comms_read(p->fd, p->active.data, &p->p_inbuf, p->active.header.length);
		if (ret == READ_SUCCESS) {
			printf("\033[1;35m[ SERVER ]\033[0m \033[1;32mserver_comms.c:\033[0m Received packet from Player \033[1;33m%s\033[0m of type \033[1;33m%d\033[0m containing \033[1;33m%d bytes\033[0m containing data:\n\t\033[1;37m%s\033[0m\n", p->name, p->active.header.type, p->active.header.length, p->active.data); 
			p->c_state = HEADER;
			p->h_inbuf = 0;
			p->p_inbuf = 0;
			p->ready = 1;
		}
		return ret;
	}
	return (ret == CLIENT_DISCONNECT) ? CLIENT_DISCONNECT : READ_FAIL;
}

// still does not have a true response
response s_listen(int max_time) {
    time_t start = time(NULL);
	int pending[LOBBY_SIZE]; // keep track of players who haven't disconnected but also haven't send a response yet
	int pending_count = 0;
	for (int i = 0; i < LOBBY_SIZE; i++) {
		pending[i] = 0;
		if (players[i].fd > 2) {
			pending[i] = 1;
			pending_count++;
		}
	}

	if (pending_count == 0) { // timeout if no players are connected
		return TIMEOUT;
	}

    // repeat for max_time time until time out or everyone has answered.
    // in case of client disconnect, you will just have to wait out the max_time.
    // however we could probably make that work better by using signals or smth?
	while (1) {
		if (pending_count == 0) {
			return TIMEOUT;
		}

        int t = time(NULL) - start;
        if (t >= max_time) {
            return TIMEOUT;
        }

        // initialize a set of file descriptors and zero them out
        fd_set fds;
        FD_ZERO(&fds);
		// highest file descriptor s.t. we can iterate through only the active file descriptors without unnecessary checks.
		// the actual performance difference is zilch though we can just iterate thru players[i], but i wrote it like ths
		// yesterday so ig we stick with it now.
        int high = 0;

        for (int i = 0; i < LOBBY_SIZE; i++) {
			if (pending[i] && players[i].fd > 2) { // only listen to players who are pending and also not disconnected
                FD_SET(players[i].fd, &fds);
                if (players[i].fd > high) {
                    high = players[i].fd;
                }
            }
        }

		if (high == 0) { // rare case of no file descriptors to listen to
			return TIMEOUT;
		}

        // set up the struct that will be used to time out if max_time has passed
        struct timeval tl; // apparently this is considered synchronous which means i need it for the select multiplexing for the timeout.
        tl.tv_sec = max_time - t;
		tl.tv_usec = 0;
		
        int listen = select(high + 1, &fds, NULL, NULL, &tl);
        if (listen > 0) {
            for (int i = 0; i < LOBBY_SIZE; i++) {
                // try to read from each player, if it reads properly then great, this was the player we wanted to listen to, otherwise, keep goin'.
				
				if (pending[i] && players[i].fd > 2 && FD_ISSET(players[i].fd, &fds)) {
					// not going to even touch reimplementing all that logic again.
					response ret = s_read(&players[i]);
					if (ret == READ_SUCCESS) {
						pending[i] = 0;
						pending_count--;
					} else if (ret == CLIENT_DISCONNECT) {
						printf("\033[1;35m[ SERVER ]\033[0m \033[1;31mserver_comms.c:\033[0m Received disconnect signal as response from \033[1;33m%s\033[0m. Disconnecting user.\n", players[i].name);
						close(players[i].fd);
						players[i].fd = -1;
						pending[i] = 0;
						pending_count--;

						if (players[i].active.data) {
							free(players[i].active.data);
							players[i].active.data = NULL;
						}
					}
				}

            }
        }
    }
}

response s_slide(int fd, Packet *p) {
	return comms_send(fd, p);
}
