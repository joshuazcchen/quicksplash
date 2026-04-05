#include "gamestructs.h"
#include "protocol.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern int PLR_COUNT;
// idk if we need this to be an actual independent copy of the string, but i think it should be ok so long as the client never modifies it.
char* ptos(Packet *p) {
    return p->data;
}

Card ptoc(Packet *p) {
    Card c;
    memset(&c, 0, sizeof(Card));
	// since the packets are funkier here but we also have an easy way to guarantee we have exactly the size we need,
	// might as well check that we have a proper packet like we do everywhere else.
	if (p->header.length > 0 && p->data != NULL) {
		char *data_copy = malloc(p->header.length + 1);
		memcpy(data_copy, p->data, p->header.length);
		data_copy[p->header.length] = '\0'; // ensure null termination

		char* token = strtok(data_copy, "\x1E"); // use the record separator from ASCII
		if (token != NULL) {
			c.prompt_text = malloc(p->header.length + 1);
			strcpy(c.prompt_text, data_copy);
		}
		c.responses = malloc(sizeof(Response*) * LOBBY_SIZE);
		for (int i = 0; i < LOBBY_SIZE; i++) {
			c.responses[i] = NULL;
		}

		int resp_id = 0;
		while ((token = strtok(NULL, "\x1E")) != NULL && resp_id < LOBBY_SIZE) {
			char* first = strchr(token, '\x1F'); // use the unit separator from ASCII
												 // basically just because we cant ever have the client use these symbols otherwise itll break.
			if (first != NULL) {
				*first = '\0';
				char* pid_str = token;
				char* str_buf = first + 1;

				char* second = strchr(str_buf, '\x1F');
				if (second != NULL) {
					*second = '\0';
					char* name = str_buf;
					char* resp_str = second + 1;
					
					c.responses[resp_id] = malloc(sizeof(Response));
					c.responses[resp_id]->player = malloc(sizeof(Player));
					memset(c.responses[resp_id]->player, '\0', sizeof(Player));
					c.responses[resp_id]->player->p_id = atoi(pid_str);
					strncpy(c.responses[resp_id]->player->name, name, 31);
					c.responses[resp_id]->response = malloc(strlen(resp_str) + 1);
					strcpy(c.responses[resp_id]->response, resp_str);
					resp_id++;
				}
			}
		}
		free(data_copy);
	}
    return c;
}

Packet ctop(Card c) {
    Packet p;
    p.header.type = PKT_CARD;
	int len = strlen(c.prompt_text) + 1;
	if (c.responses != NULL) {
		for (int i = 0; i < PLR_COUNT; i++) {
			// long statement but all it is is basically just verifying nothing we're about to use is null.
			// which would result in a segfault.
			// or many.
			// which blames everything except itself.
			if (c.responses[i] != NULL && c.responses[i]->player != NULL && c.responses[i]->response != NULL) {
				int needed = snprintf(NULL, 0, "\x1E%d\x1F%s\x1F%s", c.responses[i]->player->p_id, c.responses[i]->player->name, c.responses[i]->response);
				if (needed > 0) {
					len += needed;
				}
			}
		}
	}

	p.data = malloc(len);
	memset(p.data, 0, len);
	p.header.length = len;

	int used = 0;
	used += snprintf(p.data + used, len - used, "%s", c.prompt_text);

	if (c.responses != NULL) {
		for (int i = 0; i < PLR_COUNT; i++) {
			if (c.responses[i] != NULL && c.responses[i]->player != NULL && c.responses[i]->response != NULL) {
				used += snprintf(p.data + used, len - used, "\x1E%d\x1F%s\x1F%s", c.responses[i]->player->p_id, c.responses[i]->player->name, c.responses[i]->response);
			}
		}
	}
	printf("\033[1;35m[ SERVER ]\033[0m \033[1;32mprotocol.c:\033[0m Sending packet containing data:\n\t\033[1;37m%s\033[0m\n", p.data);
    return p;
}

// surely theres a better name for this lmfao
Packet stop(pkt_type type, char* c) {
    Packet p;
    p.header.type = type;
    //memset(p.header, '\0', sizeof(PacketHeader));
	if (c != NULL && c[0] != '\0') { // verify that theres actually something to put in the string.
		p.header.length = strlen(c) + 1;
		p.data = malloc(p.header.length);
		strcpy(p.data, c);
	} else {
		p.header.length = 22;
		p.data = malloc(sizeof(char) * 22);
		// stupid but i dont want to deal with empty packet lengths probably breaking stuff.
		// i coded in a check in the server comms iirc but its like, why even bother risking
		strcpy(p.data, "joshua/corgi was here");
	}
    return p;
}

Packet strtopkt(pkt_type type, char* c) __attribute__((alias("stop")));
char* pkttostr(Packet *p) __attribute__((alias("ptos")));
Card pkttoc(Packet *p) __attribute__((alias("ptoc")));
Packet ctopkt(Card c) __attribute__((alias("ctop")));
