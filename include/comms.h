#ifndef COMMS_H
#define COMMS_H
#include "gamestructs.h"

typedef enum {
    CLIENT_DISCONNECT = -2,
    READ_FAIL = -1,
    SEND_FAIL = -1,
    READ_PARTIAL = 0,
    READ_SUCCESS = 1,
    SEND_SUCCESS = 1,
    PARTIAL_READ_SUCCESS = 2,
    TIMEOUT = 3
} response;

// TODO: move comment from comms.c to here abt returns
int game_read(Player *p);

int game_send(int socket, Packet p);
#endif
