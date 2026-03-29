#include "socket.h"
#include "comms.h"
#include "gamestructs.h"
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

// Reads a packet sent from a given player addr p.
// Returns 1 if buffer done, 0 if read ok but not done, and -1 if something goes wrong
int game_read(Player *p) {
    int room = sizeof(Packet) - p->inbuf;
    char *after = (char*) &p->partial + p->inbuf;
    int nbytes = read(p->fd, after, room);
    
    if (nbytes <= 0) return -1;

    p->inbuf += nbytes;
    return (p->inbuf == sizeof(Packet)) ? 1 : 0;
}


