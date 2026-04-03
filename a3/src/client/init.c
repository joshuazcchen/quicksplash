#include <stdio.h>
#include <stdlib.h>
#include "socket.h"
#include "comms.h"
#include "gamestructs.h"
#include "output.h"
#include "client_comms.h"
#include "client_input.h"
#include <string.h>

int s_socket = -1;

int main() {
	if (init_display() != 0) {
		fprintf(stderr, "Error while initializing display");
		exit(1);
	}
	printf("temporary: what name? (32):\n");
	char name[32]; // how many characters do we want the maximum name legnth to be?
	get_str_to_ptr(name, 32);
	printf("temporary: what port? (7):\n");
	char port[7];
	get_str_to_ptr(port, 7);
	printf("temporary: what addr? (default 127.0.0.1):\n");
	char s_address[30];
	get_str_to_ptr(s_address, 30);
	printf("\n");
	response status = c_connect(strtol(port, NULL, 10), s_address);
	if (status == SEND_SUCCESS) {
		printf("connection successfullllererkafjksjadfk\n");
	} else {
		printf("no");
		exit(1);
	}
	return 0;
}
