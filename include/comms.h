#ifndef COMMS_H
#define COMMS_H
#include "gamestructs.h"

typedef enum {
    CLIENT_DISCONNECT = -2,
    READ_FAIL = -1,
    SEND_FAIL = -1,
    GAME_FAIL = -1,
    READ_PARTIAL = 0,
    READ_SUCCESS = 1,
    GAME_SUCCESS = 1,
    SEND_SUCCESS = 1,
    PARTIAL_READ_SUCCESS = 2,
    TIMEOUT = 3
} response;

// TODO: move comment from comms.c to here abt returns
// i do not remember AT ALL what i meant by that above comment. but it probably doesnt matter anymore because of the new system.
response comms_read(int fd, void* buf, int *inbuf, int target);

response comms_send(int fd, Packet *p);
#endif

