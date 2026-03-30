#include <stdio.h>
#include <stdlib.h>
#include "../../include/socket.h"
#include "../../include/gamestructs.h"

void start_lobby(int listenfd); // TODO: move this to a header

int main() {
    struct sockaddr_in socket = init_server_addr(30000); 
    int listenfd = socketfd;
    // i dont think i ended up using this at all lol
}
