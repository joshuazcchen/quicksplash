#ifndef COMMS_H
#define COMMS_H
#include "gamestructs.h"

// TODO: move comment from comms.c to here abt returns
int game_read(Player *p);

int game_send(int fd, Packet *p);
#endif
