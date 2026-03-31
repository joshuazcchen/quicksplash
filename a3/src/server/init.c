#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "gamestructs.h"
#include "socket.h"
#include <stdlib.h>

#define LOBBY_SIZE 5 // i swear to god ive defined this thing in like three different files. TODO: MOVE IT SOMEWHERE PERMANENTLY. IT MAKES SENSE HERE BUT NOT REALLY.
Player players[LOBBY_SIZE];

void start_lobby(int listenfd); // TODO: move this to a header

int main() {
    struct sockaddr_in *self = init_server_addr(30000); // i have zero clue why its meant to be 30,000 but the one on the csc209 lab 10 used it. if someone else knows, can they fix thsi magic num
                                                     // TODO ^^
    int listenfd = set_up_server_socket(self, 5);
    
    start_lobby(listenfd);

    free(self);
    return 0;
}
