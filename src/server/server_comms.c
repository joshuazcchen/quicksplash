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
			printf("server awaiting for type %d for %d\n", p->active.header.type, p->active.header.length); 
			p->c_state = PAYLOAD;

			if (p->active.header.length > 0) {
				p->active.data = malloc(p->active.header.length);
				if (!p->active.data) {
					perror("malloc");
					printf("serverside break\n");
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
					printf("AHHHHHHHH\n");
				}
			}
		} else { 
			return ret;
		}
	}

	if (p->c_state == PAYLOAD) {
		ret = comms_read(p->fd, p->active.data, &p->p_inbuf, p->active.header.length);
		if (ret == READ_SUCCESS) {
			printf("server read type for data %s\n", p->active.data); 
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
	int fd_count = 0;
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
		// highest file descriptor s.t. we can iterate through only the active file descriptors without unnecessary checks.
		// the actual performance difference is zilch though we can just iterate thru players[i], but i wrote it like ths
		// yesterday so ig we stick with it now.
        int high = 0;

        for (int i = 0; i < LOBBY_SIZE; i++) {
            if (players[i].fd > 2) {
                FD_SET(players[i].fd, &fds);
                if (players[i].fd > high) {
                    high = players[i].fd;
                }
            }
        }

        // set up the struct that will be used to time out if max_time has passed
        struct timeval tl; // apparently this is considered synchronous which means i need it for the select multiplexing for the timeout.
        tl.tv_sec = max_time - t;
		tl.tv_usec = 0;
		
        int listen = select(high + 1, &fds, NULL, NULL, &tl);
        if (listen > 0) {
            for (int i = 0; i < LOBBY_SIZE; i++) {
                // try to read from each player, if it reads properly then great, this was the player we wanted to listen to, otherwise, keep goin'.
				
				if (FD_ISSET(players[i].fd, &fds)) {
					// not going to even touch reimplementing all that logic again.
					response ret = s_read(&players[i]);
					if (ret == READ_SUCCESS) {
						fd_count++;
					} else if (ret == CLIENT_DISCONNECT) {
						printf("Player %s disconnected, what a loser.\n", players[i].name);
						close(players[i].fd);
						players[i].fd = -1;

						if (players[i].active.data) {
							free(players[i].active.data);
							players[i].active.data = NULL;
						}
					}
				}

            }
        }
		if(fd_count == PLR_COUNT){
			return TIMEOUT;
		}
    }
}

response s_slide(int fd, Packet *p) {
	return comms_send(fd, p);
}

// TODO: make clear responses call apparently? idk what that means still
//
