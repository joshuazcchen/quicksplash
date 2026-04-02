#include "comms.h"
#include "gamestructs.h"
#include <string.h>

extern int socket; // this is set during the initial connection

Packet partial;
int inbuf = 0;
Packet active;
int ready = 0;

response c_send(Packet p) {
    return comms_send(socket, p);
}

response c_read() {
    ret = comms_read(socket, &partial, &inbuf);
    if (ret == READ_SUCCESS) {
        memcpy(&active, &partial, sizeof(Packet));
        inbuf = 0;
        ready = 1;
    } else {
        return response;
    }
    return ret;
}


