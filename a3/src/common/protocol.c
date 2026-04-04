#include "gamestructs.h"
#include "protocol.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// idk if we need this to be an actual independent copy of the string, but i think it should be ok so long as the client never modifies it.
char* ptos(Packet *p) {
    //p->data[BUFFERSIZE-1] = '\0';
	// woah no more set buffersize!!! ^^^ 
	// if this is all i need i will eat a shoe but i do genuinely think this is all i need LOL.
    return p->data;
}

Card ptoc(Packet *p) {
    Card c;
    memset(&c, 0, sizeof(Card));
	// since the packets are funkier here but we also have an easy way to guarantee we have exactly the size we need,
	// might as well check that we have a proper packet like we do everywhere else.
	if (p->header.length > 0 && p->data != NULL) {
		c.prompt_text = malloc(p->header.length);
		strcpy(c.prompt_text, p->data);
	}
    return c;
}

Packet ctop(Card c) {
    Packet p;
    p.header.type = PKT_CARD;
    //memset(&p.header, '\0', sizeof(PacketHeader));
	p.header.length = strlen(c.prompt_text) + 1;
	p.data = malloc(p.header.length);
	strcpy(p.data, c.prompt_text);
	printf("sending packet string: n %s \n", p.data);
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
