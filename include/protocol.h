#ifndef PROTOCOL_H
#define PROTOCOL_H
#include "gamestructs.h"

char* ptos(Packet *p);
Card ptoc(Packet *p);
Packet ctop(Card c);
Packet stop(pkt_type type, char* c);

#endif
