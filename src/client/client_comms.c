#include "comms.h"
#include "client_comms.h"
#include "gamestructs.h"
#include <string.h>
#include "socket.h"
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

extern int s_socket;

Packet active;
pkt_state c_state = HEADER;
int h_inbuf = 0;
int p_inbuf = 0;
int ready = 0;

response c_send(Packet *p) {
    return comms_send(s_socket, p);
}

response c_read() {
	response ret = READ_FAIL; // make default value so make small doesnt warn
	// read header 
	if (c_state == HEADER) {
		ret = comms_read(s_socket, &active.header, &h_inbuf, sizeof(PacketHeader));
		if (ret == READ_SUCCESS) {
			active.header.length = ntohs(active.header.length); // convert back from htons to the local machines short, will require changing to the ntohl if we end up using int64 or smth else.
			c_state = PAYLOAD;
			if (active.header.length > 0) {
				active.data = malloc(active.header.length);
				if (!active.data) {
					printf("\033[1;36m[ CLIENT ]\033[0m \033[1;31mclient_comms.c:\033[0m Failed to read header.\n\t"); // i dont think i actually get an errno here because technically nothing sys went wrong.
					perror("malloc");
					exit(1); // if this hapepns then the client will die anyways, so might as well die on our terms.
							 // since we'd segfault faster than ever if it fails to alloc data.
				}
			} else {
				// malformed packet header or basically just sending a signal.
				// either way pretty useless.
				if (active.header.length == 0) {
					active.data = NULL;
					c_state = HEADER;
					h_inbuf = 0;
					ready = 1;
					return READ_SUCCESS; // dont even bother reading and just parse thru that it was a success if its == 0 len.
				} else {
					printf("\033[1;36m[ CLIENT ]\033[0m \033[1;31mclient_comms.c:\033[0m Malformed header.\n"); // i dont think i actually get an errno here because technically nothing sys went wrong.
					exit(1); // this client is sending malformed headers so kill it before it can do more harm.
				}
			} 
			return ret;
		}
	}
	// NOTE TO SELF: I MIGHT NEED A SLEEP HERE IM NOT SURE HOW QUICKLY I CAN SEND PACKETS IN SUCCESSION, I ASSUME THAT ITLL BE FINE SINCE THE PACKET AND PAYLOAD ARE EFFECTIVELY SENT RIGHT AFTER ONE ANOTHER.

	// read payload
	if (c_state == PAYLOAD) {
		ret = comms_read(s_socket, active.data, &p_inbuf, active.header.length);
		if (ret == READ_SUCCESS) {
			// if it succeeds we can just reset
			c_state = HEADER;
			h_inbuf = 0;
			p_inbuf = 0;
			ready = 1;
		}
		return ret;
	}

	return ret == CLIENT_DISCONNECT ? CLIENT_DISCONNECT : READ_FAIL;
}


// big change to this one for the change so this is gonna be deprecated too.
//response c_read() {
//    response ret = comms_read(s_socket, &partial, &inbuf);
//    if (ret == READ_SUCCESS) {
//        active = partial;
//        inbuf = 0;
//        ready = 1;
//    }
//
//    return ret;
//}

response c_connect(int port, char* addr) {
	int s = connect_to_server(port, addr);
	if (s >= 0) {
		s_socket = s;
		return SEND_SUCCESS;
	} else {
		return SEND_FAIL;
	}
}
