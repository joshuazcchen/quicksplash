#include "gamestructs.h"
#include "protocol.h"
#include <string.h>

// idk if we need this to be an actual independent copy of the string, but i think it should be ok so long as the client never modifies it.
char* ptos(Packet *p) {
    p->data[BUFFERSIZE-1] = '\0';
    return p->data;
}

Card ptoc(Packet *p) {
    Card c;
    memcpy(&c, p->data, sizeof(Card));
    return c;
}

Packet ctop(Card c) {
    Packet p;
    p.type = P_CARD;
    memset(p.data, '\0', BUFFERSIZE);
    memcpy(p.data, &c, sizeof(Card));
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
