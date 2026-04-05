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
		} else {
			printf("Error in sending join packet\n");
			exit(1);
		}
		free(p.data);
		sleep(2);
		Packet pst = stop(PKT_START, "orange");
		if (c_send(&pst) == SEND_SUCCESS) {
			printf("started game\n");
		} else {
			printf("bad packet");
		}
		free(pst.data);
		while (1) {
			sleep(1);
			if (c_read() == READ_SUCCESS && ready) {
				printf("received packet of %d %d from server\n", active.header.type, active.header.length);
				printf("JOSHUA UR SHIT IS SEGFALUTING");
				Card rec = pkttoc(&active);
				printf("\ncard got: %s\n", rec.prompt_text ? rec.prompt_text : "[no text]");

				if (active.header.type == PKT_CARD) {
					char *prompt_response = get_card_prompt_response(rec, MAX_RESPONSE_SIZE - 1);
					Packet reply = strtopkt(PKT_REPLY, prompt_response);
					c_send(&reply);
					free(prompt_response);
					free(reply.data);
				} else if (active.header.type == PKT_VOTE) {
					printf("DOES THIS WORK?");
					for(int i = 0; i < LOBBY_SIZE; i++){
						if (rec.responses[i] != NULL && rec.responses[i]->response != NULL) {
							printf("client recieved pid %d and with their response as %s \n",rec.responses[i]->player->p_id, rec.responses[i]->response);
						} else {
							printf("no\b");
						}
					}
					// show_vote_card(rec, LOBBY_SIZE);
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
//	printf("temporary: what name? (32):\n");
//	char name[32]; // how many characters do we want the maximum name legnth to be?
//	get_str_to_ptr(name, 32);
//	printf("temporary: what port? (7):\n");
//	char port[7];
//	get_str_to_ptr(port, 7);
//	printf("temporary: what addr? (default 127.0.0.1):\n");
//	char s_address[30];
//	get_str_to_ptr(s_address, 30);
//	printf("\n");
//	// connects
//	response status = c_connect(strtol(port, NULL, 10), s_address);
//	if (status == SEND_SUCCESS) {
//		Packet p = stop(P_JOIN, name); // once again we need a better name for this, string to packet.	
//		if (c_send(p) == SEND_SUCCESS) {
//			printf("great ok client ready\n");
//		} else {
//			printf("bad\n");
//		}
//		sleep(2);
//		Packet pst = stop(P_START, "orange");
//		if (c_send(pst) == SEND_SUCCESS) {
//			printf("started\n");
//		} else {
//			printf("bad");
//		}
//		while (1) {
//			sleep(1);
//			if (c_read() == READ_SUCCESS && ready) {
//				printf("%s\n", ptoc(&active).prompt_text);
//			}
//		}
//	} else {
//		printf("no\n");
//		exit(1);
//	}
	return 0;
}
