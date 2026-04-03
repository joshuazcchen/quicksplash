#include "comms.h"
#include "client_comms.h"
#include "gamestructs.h"
#include <string.h>
#include "socket.h"

Packet partial;
int inbuf = 0;
Packet active;
int ready = 0;

response c_send(Packet p) {
    return comms_send(s_socket, p);
}

response c_read() {
    response ret = comms_read(s_socket, &partial, &inbuf);
    if (ret == READ_SUCCESS) {
        memcpy(&active, &partial, sizeof(Packet));
        inbuf = 0;
        ready = 1;
    }
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
