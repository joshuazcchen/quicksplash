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
	int port = (rand() % (65000 - 30001) + 30000); 
    struct sockaddr_in *self = init_server_addr(port);
    int listenfd = set_up_server_socket(self, 5);
    
    start_lobby(listenfd);
    printf("Lobby ready -> proceeding to game\n");

    if (setup_game() == GAME_SUCCESS){
        printf("CARDS INITIALIZED\n");
    }

    if(game_loop(2) == GAME_SUCCESS){
       printf("GOOD!\n");
    }

    if(determine_game_winner()==GAME_SUCCESS){
         printf("game winner picked!\n");
    }

    if (wrap_up_game() == GAME_SUCCESS){
        printf("GAME ENDED WOWWWWWWWWWWWWWWWWWWWWWWWWW\n");
    }

    free(self);
    return 0;
}
