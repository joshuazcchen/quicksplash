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
#include <sys/select.h>
#include <signal.h>

int s_socket = -1;
extern Packet active;
extern int ready; 
int PLR_COUNT = 0; // this just guarantees that both the client and server have the same value and can be used in the same comms functions. maybe not the best structure but at this point just needs to work.
				   // Will be synced on join and we will make a disconnect also send a message to the client as necessary.

				   // BEHOLD, MY SIGINTINATOR, WITH THIS DEVICE, EVERY SIGINT SHALL BE HANDLED! Unless you disconnect without a sigint like your internet dies or something.
void sigintinator(int sig) {
	printf("\n\n\033[1;36m[ CLIENT ]\033[0m \033[1;31minit.c:\033[0m SIGINT received, exiting with code %d. Imagine ragequitting. Removing you from server\n", sig); // makefile is annoying me with the "unused variable!!!" so its here now.
	if (s_socket != -1) {
		Packet p = strtopkt(PKT_QUIT, "i ragequit");
		c_send(&p); // we dont even need to check this, because if this fails it doesnt even matter.
					// server will just handle it as a disconnect.
					// but this is funnier if it works.
		free(p.data);
		close(s_socket);
	}
	exit(0);
}

// BEHOLD, MY SIGINTINATORINATOR, IT SIGINTINATES THE SIGINTINATOR FOR SINGINTINATING THE SIGINTINATIONATORINATING
void __attribute__((alias("sigintinator"))) sigintinatorinator(int sig);

int main() {
	signal(SIGINT, sigintinatorinator); // this is so stupid i dont think i got enough sleep last night.
	if (init_display() != 0) {
		fprintf(stderr, "Error while initializing display");
		exit(1);
	}
	// temporary, but gets the users name, the port they want to connect to and their address they want to connect to
	clear_screen();
	char name[32] = {0};  // remove valgrind issues with calling strlen() on empty/uninitialized strings
	char port[7] = {0};
	char s_address[30] = {0};
	int host = 0; // had to move host variable to this scope instead for the rest to work properly

	server_select(name, port, s_address);
	int s_port = strtol(port, NULL, 10);

	response status = c_connect(s_port, s_address);
	if (status == SEND_SUCCESS) {
		Packet p = strtopkt(PKT_JOIN, name);
		if (c_send(&p) == SEND_SUCCESS) {
			printf("\033[1;36m[ CLIENT ]\033[0m \033[1;32minit.c:\033[0m Joined as \033[1;33m%s\033[0m.\n", name);
			free(p.data);

			ready = 0;
			while (!ready) {
				if (c_read() == CLIENT_DISCONNECT) {
					printf("\033[1;35m[ SERVER ]\033[0m \033[1;31mserver_comms.c:\033[0m Connection dropped.\n");
					exit(1);
				}
			}
			if (strcmp(pkttostr(&active), "YOU ARE HOST NOW CONGRATULATIONS") == 0) {
				host = 1;
				printf("\033[1;35m[ SERVER ]\033[0m \033[1;32mlobby.c:\033[0m You are now the \033[1;33mHost\033[0m.\n\t\033[1;37mPress any key to start the game.\033[0m\n");
				fflush(stdout);
			} else {
				printf("\033[1;35m[ SERVER ]\033[0m \033[1;32mlobby.c:\033[0m You are now a \033[1;33mGuest\033[0m.\n\t\033[1;37mPlease wait for the Host to start the game.\033[0m\n");
			}

			if (active.data != NULL) {
				free(active.data);
				active.data = NULL;
			}
			ready = 0;

			fd_set fds;
			while (1) {
				FD_ZERO(&fds);
				FD_SET(STDIN_FILENO, &fds);
				FD_SET(s_socket, &fds);

				// stupidly c doesnt have a max function
				int max_fd = (s_socket > STDIN_FILENO) ? s_socket : STDIN_FILENO;

				if (select(max_fd + 1, &fds, NULL, NULL, NULL) > 0) {
					if (FD_ISSET(STDIN_FILENO, &fds)) {
						// honestly i dont care what the hell they send but if we want to later on we can just make it require them to send a specific start message. right now, this will just intercept if they say ANYTHING at all.
						char c;
						while ((c= getchar()) != '\n' && c != EOF);
						if (!host) continue;

						Packet pst = strtopkt(PKT_START, "orange");
						printf("\033[1;36m[ CLIENT ]\033[0m \032[1;33minit.c:\033[0m You have sent the start packet to the \033[1;35mServer\033[0m.\n");
						if (c_send(&pst) == SEND_SUCCESS) {
							printf("\033[1;35m[ SERVER ]\033[0m \033[1;32mlobby.c:\033[0m Received start packet. Starting game.\n");
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
							ready = 0;
							if (active.header.type == PKT_START) {
								//char* val = pkttostr(&active);
								PLR_COUNT = atoi(pkttostr(&active)); 
								free(active.data);
								active.data = NULL;
								break;
							}
						}
					}
				}
			}
		} else {
			printf("\033[1;36m[ CLIENT ]\033[0m \033[1;31minit.c:\033[0m Failed to send join packet.\n");
			exit(1);
		}

		while (1) {
			response ret = c_read();
			if (ret == CLIENT_DISCONNECT) {
				printf("\033[1;36m[ CLIENT ]\033[0m \033[1;31minit.c:\033[0m Connection dropped.\n");
				close(s_socket);
				exit(1);
			}
			// ALSO TODO: make sure that when vote is sent it is always just a single int
			if (ret == READ_SUCCESS && ready) {
				if (active.header.type == PKT_CARD) {
					Card rec = pkttoc(&active);
					char *prompt_response = get_card_prompt_response(rec, MAX_RESPONSE_SIZE - 1);
					Packet reply = strtopkt(PKT_REPLY, prompt_response);
					c_send(&reply);
					free(prompt_response);
					free(reply.data);
					if (rec.prompt_text != NULL) free(rec.prompt_text);

					// this is effectively the same code as in the free card, but basically just moved to here.
					// frees responses attached to the card to clear out the 40byte memory leak
					if (rec.responses != NULL) {
						for (int i = 0; i < LOBBY_SIZE; i++) {
							if (rec.responses[i] != NULL) {
								if (rec.responses[i]->response != NULL) {
									free(rec.responses[i]->response);
								}
								if (rec.responses[i]->player != NULL) {
									// we can clear players here because we dont rlly care, at this point if they need the players prompts again they can get it from the server.
									free(rec.responses[i]->player); // clean up on the clientside
								}
								free(rec.responses[i]);
							}
						}
						free(rec.responses);
					}
				} else if (active.header.type == PKT_VOTE) {
					Card rec = pkttoc(&active);
					show_vote_card(rec, LOBBY_SIZE);
					char vote_response[8];
					int index = -1;
					int vote_state = 0; // 0 = waiting, 1 = sent, -1 = results packet arrived

					while (vote_state == 0) { // to not block on user input, we can loop and use file descriptors and select with packet listening
						fd_set vote_fds;
						FD_ZERO(&vote_fds); // we need to reset the vote file descriptors first
						FD_SET(STDIN_FILENO, &vote_fds); // listen for user input to select a vote
						FD_SET(s_socket, &vote_fds); // listen for incoming packets

						int max_fd = (s_socket > STDIN_FILENO) ? s_socket : STDIN_FILENO; // either the socket or the stdin, whichever is higher, for select
						if (select(max_fd + 1, &vote_fds, NULL, NULL, NULL) <= 0) { // wait until either user input or a packet is received
							continue;
						}

						if (FD_ISSET(s_socket, &vote_fds)) { // if we receive a packet but still waiting for user input
							response socket_ret = c_read(); // process it
							if (socket_ret == CLIENT_DISCONNECT) { // timeout? could be any other disconnect tho
								printf("\033[1;36m[ CLIENT ]\033[0m \033[1;31minit.c:\033[0m Connection dropped.\n");
								close(s_socket); // we should now close it LMFAO
								exit(1);
							}

							if (socket_ret == READ_SUCCESS && ready) { // if we got a packet and its ready to be processed
								if (active.header.type == PKT_STATS) { // vote results!!!
									show_vote_results(pkttostr(&active)); // show them!!!
									if (host) {
										printf("\n\033[1;33mPress Enter to continue to the next round...\033[0m");
										char wait_buf[8];
										get_str_to_ptr(wait_buf, sizeof(wait_buf));
										Packet cont = strtopkt(PKT_START, "continue");
										c_send(&cont);
										free(cont.data);
									}
									vote_state = -1; // we can break out of the loop now because we got the results packet, we dont need to listen for user input anymore
								} else if (active.header.type == PKT_GAME_END) { // uh, game ended :(
									printf("\033[1;35m[ SERVER ]\033[0m \033[1;32mserver_comms.c:\033[0m Received game over packet.\n");
									if (active.data != NULL) free(active.data); // let's not forget to do that lol (memory leaks)
									exit(0);
								}
							}
						}

						if (vote_state != 0) {
							break;
						}

						if (FD_ISSET(STDIN_FILENO, &vote_fds)) { // if user input is received and we are still waiting for vote results to come
							get_str_to_ptr(vote_response, sizeof(vote_response)); // process user input
							index = strtol(vote_response, NULL, 10) - 1; // convert to index (user will input 1-5, we need 0-4)

							if (index >= 0 && index < LOBBY_SIZE && rec.responses[index] != NULL && rec.responses[index]->player != NULL) {
								int pid = rec.responses[index]->player->p_id;
								char pid_str[12];
								snprintf(pid_str, sizeof(pid_str), "%d", pid);
								Packet vote = strtopkt(PKT_VOTE,pid_str);
								c_send(&vote);
								free(vote.data);
								vote_state = 1;
							} else {
								printf("Invalid choice. Please enter a valid response number: ");
							}
						}
					}

					//TODO: make the player select one ofthe responses, once they select a value send back the PID that represents that player response 
					// for example rec.responses[i]->player->p_id represenrts first entry in vote, player selects 1 then send back rec.responses[i]->player->p_id to server
					if (rec.prompt_text != NULL) free(rec.prompt_text);

					// this is effectively the same code as in the free card, but basically just moved to here.
					// frees responses attached to the card to clear out the 40byte memory leak
					if (rec.responses != NULL) {
						for (int i = 0; i < LOBBY_SIZE; i++) {
							if (rec.responses[i] != NULL) {
								if (rec.responses[i]->response != NULL) {
									free(rec.responses[i]->response);
								}
								if (rec.responses[i]->player != NULL) {
									// we can clear players here because we dont rlly care, at this point if they need the players prompts again they can get it from the server.
									free(rec.responses[i]->player); // clean up on the clientside
								}
								free(rec.responses[i]);
							}
						}
						free(rec.responses);
					}
				} else if (active.header.type == PKT_STATS) {
						show_vote_results(pkttostr(&active));
						if (host) {
							printf("\n\033[1;33mPress Enter to continue to the next round...\033[0m");
							char wait_buf[8];
							get_str_to_ptr(wait_buf, sizeof(wait_buf));
							Packet cont = strtopkt(PKT_START, "continue");
							c_send(&cont);
							free(cont.data);
						}
				} else if (active.header.type == PKT_GAME_END) {
					printf("\033[1;35m[ SERVER ]\033[0m \033[1;32mserver_comms.c:\033[0m Received game over packet.\n");
					if (active.data != NULL) free(active.data);
					exit(0);
				}

	//			if (rec.prompt_text != NULL) free(rec.prompt_text);

				// this is effectively the same code as in the free card, but basically just moved to here.
				// frees responses attached to the card to clear out the 40byte memory leak
	//			if (rec.responses != NULL) {
	//				for (int i = 0; i < LOBBY_SIZE; i++) {
	//					if (rec.responses[i] != NULL) {
	//						if (rec.responses[i]->response != NULL) {
	//							free(rec.responses[i]->response);
	//						}
	//						if (rec.responses[i]->player != NULL) {
	//							// we can clear players here because we dont rlly care, at this point if they need the players prompts again they can get it from the server.
	//							free(rec.responses[i]->player); // clean up on the clientside
	//						}
	//						free(rec.responses[i]);
	//					}
	//				}
	//				free(rec.responses);
	//			}

				free(active.data);
				active.data = NULL;
				ready = 0;
			}
		}
	}
	return 0;
}
