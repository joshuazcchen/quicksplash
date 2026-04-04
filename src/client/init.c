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
	char name[32];
	char port[7];
	char s_address[30];

	server_select(name, port, s_address);
	int s_port = strtol(port, NULL, 10);

	response status = c_connect(s_port, s_address);
	if (status == SEND_SUCCESS) {
		Packet p = stop(P_JOIN, name);
		if (c_send(p) == SEND_SUCCESS) {
			printf("Joined as %s\n", name);
		} else {
			printf("Error in sending join packet\n");
			exit(1);
		}
		sleep(2);
		Packet pst = stop(P_START, "orange");
		if (c_send(pst) == SEND_SUCCESS) {
			printf("started game\n");
		} else {
			printf("bad packet");
		}
		while (1) {
			sleep(1);
			if (c_read() == READ_SUCCESS && ready) {
				Card rec = ptoc(&active);
				printf("\ncard got: %s\n", rec.prompt_text);
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
