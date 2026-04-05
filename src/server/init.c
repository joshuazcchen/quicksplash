#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "gamestructs.h"
#include "socket.h"
#include <stdlib.h>
#include "comms.h"
#include "gamestates.h"
#include <unistd.h>
#include "server_comms.h"
#include "protocol.h"
#include "lobby.h"
#include <signal.h>

Player players[LOBBY_SIZE];

int main() {
	signal(SIGPIPE, SIG_IGN);
    struct sockaddr_in *self = init_server_addr(30000); // i have zero clue why its meant to be 30,000 but the one on the csc209 lab 10 used it. if someone else knows, can they fix thsi magic num
                                                     // TODO ^^
    int listenfd = set_up_server_socket(self, 5);
    
    start_lobby(listenfd);
    printf("Lobby ready -> proceeding to game\n");

    if (setup_game() == GAME_SUCCESS){
        printf("CARDS INITIALIZED\n");
    }

    if(game_loop(1) == GAME_SUCCESS){
       printf("GOOD!\n");
    }

    if (wrap_up_game() == GAME_SUCCESS){
        printf("GAME ENDED WOWWWWWWWWWWWWWWWWWWWWWWWWW\n");
    }

    free(self);
    return 0;
}
