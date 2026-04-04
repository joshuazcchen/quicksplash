#include "gamestructs.h"
#include "protocol.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// idk if we need this to be an actual independent copy of the string, but i think it should be ok so long as the client never modifies it.
char* ptos(Packet *p) {
    p->data[BUFFERSIZE-1] = '\0';
    return p->data;
}

Card ptoc(Packet *p) {
    Card c;
    memset(&c, 0, sizeof(Card));
    c.prompt_text = malloc(strlen(p->data) + 1);
    strcpy(c.prompt_text, p->data);
    return c;
}

Packet ctop(Card c) {
    Packet p;
    p.type = P_CARD;
    memset(p.data, '\0', BUFFERSIZE);
    strncpy(p.data, c.prompt_text, sizeof(char)*(strlen(c.prompt_text)+1));
    printf("sending packet string: \n %s \n",p.data);
    return p;
}

// surely theres a better name for this lmfao
Packet stop(p_type type, char* c) {
    Packet p;
    p.type = type;
    memset(p.data, '\0', BUFFERSIZE);
    strncpy(p.data, c, BUFFERSIZE);
    p.data[BUFFERSIZE-1] = '\0';
    return p;
}

