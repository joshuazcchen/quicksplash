#include "socket.h"
#include "comms.h"
#include "protocol.h"
#include "gamestructs.h"
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h> // this is needed to do the ntohs and such i think htons

// read from socket:
// @params
// int fd: the file descriptor
// void *buf: a void pointer because it can technically read anything just bc of the fact that voting should transmit ints
// int *inbuf: address to the current fd's amt already read
// int target: the total chars/data in payload
response comms_read(int fd, void *buf, int *inbuf, int target) {
    int room = target - *inbuf;
    if (room <= 0) {
        return READ_SUCCESS; // yooooo in this case it literally means a SUCCESS not a FAILURE anymore hahahahahaha
    }
    int nbytes = recv(fd, (char*)buf + *inbuf, room, MSG_DONTWAIT);
    if (nbytes == 0) {
        return CLIENT_DISCONNECT; // if we dont receive anything, clients probably dead so lets treat them as dead
								  // this is not different from the old versio njsut commenting in now so that its easier to
								  // understand later on.
    } else if (nbytes < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return READ_PARTIAL;
        } 
        return CLIENT_DISCONNECT; // same kind of error handling as before, at least on intiial passthrough these seem like it
								  // would all still work?
    }
    *inbuf += nbytes;
    if (*inbuf == target) {
        return READ_SUCCESS;
    }
    return READ_PARTIAL;
}

// only big difference in handling is that this now strictly takes an address to a packet. this should have been the case on the old one too but im stupid and didnt think ahead at the time.
// ok in hindsight maybe the only big change was just that I had to send it in two separate sets? 
response comms_send(int fd, Packet *p) {
	PacketHeader p_head;

	memset(&p_head, '\0', sizeof(PacketHeader));

	p_head.type = p->header.type;
	// if we end up swapping to a larger maximum packet size we may need to convert this to htonl (host to network long) compared to currently hton(short).
	p_head.length = htons(p->header.length); // seems this handles different cpu architectures (little vs big endian).
											 // Also not sure how necessary this is but thre big people online are big
											 // on "if you dont have this your code will explode".
	if (write(fd, &p_head, sizeof(PacketHeader)) < 0) {
		perror("comms_send");
		printf("something broke while sending the header;");
		return SEND_FAIL;
	}

	// send actual payload
	if (p_head.length > 0 && p->data) {
		if (write(fd, p->data, p->header.length) < 0) {
			perror("comms_send");
			printf("something broke while sending the hydrogen bomb;");
			return SEND_FAIL;
		}
	}

	return SEND_SUCCESS;
}

// this is one thatll get quite a big change so im just going to rewrite it and deprecate the old one.
//response comms_send(int fd, Packet p) {
//    if (write(fd, &p, sizeof(p)) < 0) {
//        perror("comms_send");
//        return SEND_FAIL;
//    }
//    return SEND_SUCCESS;
//}
