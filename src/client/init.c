#include <stdio.h>
#include <stdlib.h>
#include "../../include/socket.h"
#include "../../include/gamestructs.h"
#include "../../include/output.h"

void start_lobby(int listenfd); // TODO: move this to a header

int main() {
    if (init_display() != 0) {
      fprintf(stderr, "Error while initializing display");
      exit(1);
    }
    struct sockaddr_in socket = init_server_addr(30000); 
    int listenfd = socketfd;
    // i dont think i ended up using this at all lol
}
