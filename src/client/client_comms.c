#include "comms.h"
#include "client_comms.h"
#include "gamestructs.h"
#include <string.h>
#include "socket.h"
#include <stdlib.h>
#include <stdio.h>

extern int s_socket;

Packet partial;
int inbuf = 0;
Packet active;
int ready = 0;

response c_send(Packet p) {
    return comms_send(s_socket, p);
}

response c_read() {
    Packet *pptr = malloc(sizeof(Packet));
    Packet *aptr = malloc(sizeof(Packet));
    memset(aptr, '\0', sizeof(Packet));
    memset(pptr, '\0', sizeof(Packet));
    if (!inbuf) {
        inbuf = 0;
    }

    response ret = comms_read(s_socket, pptr, &inbuf);
    if (ret == READ_SUCCESS) {
        memcpy(aptr, pptr, sizeof(Packet));
        active = *aptr;
        partial = *pptr;
        inbuf = 0;
        ready = 1;
    }

    free(pptr);
    free(aptr);
    return ret;
}

response c_connect(int port, char* addr) {
    int s = connect_to_server(port, addr);
    if (s) {
        s_socket = s;
        return SEND_SUCCESS;
    } else {
        return SEND_FAIL;
    }
}
