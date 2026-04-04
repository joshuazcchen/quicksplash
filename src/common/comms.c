#include "socket.h"
#include "comms.h"
#include "protocol.h"
#include "gamestructs.h"
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// migrating stuff over to use response instead of ints for error logging bc i think it looks more professional rn. i think you can work with both basically the same way tho so it doesnt rlly make a diff.
response comms_read(int fd, Packet *partial, int *inbuf) {
    int room = sizeof(Packet) - *inbuf;
    if (room <= 0) {
        return READ_FAIL;
    }
    int nbytes = recv(fd, (char*)partial + *inbuf, room, MSG_DONTWAIT);
    if (nbytes == 0) {
        return CLIENT_DISCONNECT;
    } else if (nbytes < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return READ_PARTIAL;
        } 
        return CLIENT_DISCONNECT;
    }
    *inbuf += nbytes;
    if (*inbuf == sizeof(Packet)) {
        return READ_SUCCESS;
    }
    return READ_PARTIAL;
}

response comms_send(int fd, Packet p) {
    if (write(fd, &p, sizeof(p)) < 0) {
        perror("comms_send");
        return SEND_FAIL;
    }
    return SEND_SUCCESS;
}

