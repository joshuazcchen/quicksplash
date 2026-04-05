#include <stdio.h>
#include <stdlib.h>
#include "socket.h"
#include "comms.h"
#include "gamestructs.h"
#include "output.h"
#include "client_comms.h"
#include "client_input.h"
#include "protocol.h"
#include <string.h>
#include <unistd.h>

int s_socket = -1;
extern Packet active;
extern int ready; 

int main() {
	if (init_display() != 0) {
		fprintf(stderr, "Error while initializing display");
		exit(1);
	}
	// temporary, but gets the users name, the port they want to connect to and their address they want to connect to
	clear_screen();
	char name[32] = {0};  // remove valgrind issues with calling strlen() on empty/uninitialized strings
	char port[7] = {0};
	char s_address[30] = {0};

	server_select(name, port, s_address);
	int s_port = strtol(port, NULL, 10);

	response status = c_connect(s_port, s_address);
	if (status == SEND_SUCCESS) {
		Packet p = strtopkt(PKT_JOIN, name);
		if (c_send(&p) == SEND_SUCCESS) {
			printf("Joined as %s\n", name);
			free(p.data);
			
			ready = 0;
			while (!ready) {
				c_read();
			}
			int host = 0;
			printf("%s\n", pkttostr(&active));
			if (strcmp(pkttostr(&active), "YOU ARE HOST NOW CONGRATULATIONS") == 0) {
				host = 1;
				printf("you are host\n");
				fflush(stdout);
			} else {
				printf("you are not host\n");
			}

			if (active.data != NULL) {
				free(active.data);
				active.data = NULL;
			}

			fd_set(fds);
			while (1) {
				FD_ZERO(&fds);
				FD_SET(STDIN_FILENO, &fds);
				FD_SET(s_socket, &fds);

				// stupidly c doesnt have a max function
				int max_fd = (s_socket > STDIN_FILENO) ? s_socket : STDIN_FILENO;

				if (select(max_fd + 1, &fds, NULL, NULL, NULL) > 0) {
					if (FD_ISSET(STDIN_FILENO, &fds)) {
						// honestly i dont care what the hell they send but if we want to later on we can just make it require them to send a specific start message. right now, this will just intercept if they say ANYTHING at all.
						if (!host) continue;
						Packet pst = strtopkt(PKT_START, "orange");
						if (c_send(&pst) == SEND_SUCCESS) {
							printf("game started\n");
							char c;
							while ((c= getchar()) != '\n' && c != EOF);
							host = 0; // we dont need a host anymore.
						}
						free(pst.data);
						break;
					}

					// in the event of server full or smth like that then just send the host a message to let them 
					// know to stop waiting for
					// a response from the user designated host
					//
					// this also breaks non host players out of the loop bc they always end up here.
					if (FD_ISSET(s_socket, &fds)) {
						c_read();
						if (ready) {
							break;
						}
					}
				}
			}
		} else {
			printf("Error in sending join packet\n");
			exit(1);
		}
		//Packet pst = stop(PKT_START, "orange");
		
		while (1) {
			sleep(1);
			if (c_read() == READ_SUCCESS && ready) {
				printf("received packet of %d %d from server\n", active.header.type, active.header.length);
				Card rec = pkttoc(&active);
				printf("\ncard got: %s\n", rec.prompt_text ? rec.prompt_text : "[no text]");

				if (active.header.type == PKT_CARD) {
					char *prompt_response = get_card_prompt_response(rec, MAX_RESPONSE_SIZE - 1);
					Packet reply = strtopkt(PKT_REPLY, prompt_response);
					c_send(&reply);
					free(prompt_response);
					free(reply.data);
				} else if (active.header.type == PKT_VOTE) {
					for(int i = 0; i < LOBBY_SIZE; i++){
						if (rec.responses[i] != NULL && rec.responses[i]->response != NULL) {
							printf("client recieved pid %d and with their response as %s \n",rec.responses[i]->player->p_id, rec.responses[i]->response);
						} else {
							printf("no\n");
						}
					}
					// so this still causes a crash TODO kai
					//show_vote_card(rec, LOBBY_SIZE);
				} else {
					printf("\npacket type %d data: %s\n", active.header.type, rec.prompt_text ? rec.prompt_text : "");
				}

				if (rec.prompt_text) free(rec.prompt_text);
				free(active.data);
				active.data = NULL;
				ready = 0;
			}
		}
	}
	return 0;
}
