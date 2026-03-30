#include "socket.h"
#include "comms.h"
#include "gamestructs.h"
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// WHOEVER ENDS UP IMPLEMENTING A PROPER GAME_SEND NEEDS TO REMEMBER TO ALWAYS HAVE PRECONDITION WHERE FIRST FOUR BYTES ARE THE TYPE FOR THE INITIAL THING
// nvm i ended up implementing game_send
int s_send(int server, int type, char *data) {
    game_send(server, create_packet(type, data));
}

int game_send(int server, Packet p) {
    write(server, &p, sizeof(Packet));
}

Packet create_packet(int type, char *data) {
    Packet p;
    p.type = type;
    strcpy(p1.data, data);
}

// Reads a packet sent from a given player addr p.
// Returns 1 if buffer done, 0 if read ok but not done, and <=-1 if something goes wrong
int game_read(Player *p) {
    int room = sizeof(p->partial) - p->inbuf;
    if (room <= 0) {
        return -1; // we need better error logging, currently rewriting this entire set of functions so it actually has error checking and handles different packet types properly rn.
    }
    
    int nbytes;
    if (p->inbuf > 0) {
        nbytes = recv(p->fd, (char*) &p->partial + p->inbuf, room, MSG_DONTWAIT);
        if (nbytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            nbytes = 0;
        } else if (nbytes <= 0) {
            return -2;
        }
    } else {
        nbytes = read(p->fd, (char*) &p->partial + p->inbuf, room);
        if (nbytes <= 0) { return -2; }
    }
    p->inbuf += nbytes;
    if (p->inbuf == sizeof(Packet)) {
        for (int i = 0; i < BUFFERSIZE - 1; i++) {
            if (p->partial.data[i] == '\r' && p->partial.data[i+1] == '\n') {
                p->partial.data[i] = '\0';
                break;
            }
        }
        p->ready = 1;
        p->inbuf = 0;
        return 1;
    }

    return 0;
}
