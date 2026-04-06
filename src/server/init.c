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
#include <time.h> // for the seed!!!

Player players[LOBBY_SIZE];

int main() {
    printf("\033[1;35m[ SERVER ]\033[0m \033[1;32minit.c:\033[0m Starting server.");
	signal(SIGPIPE, SIG_IGN);

	srand(time(NULL)); // seed the time so that we stop getting port 50935
	int port = (rand() % (65000 - 30001) + 30000);
    struct sockaddr_in *self = init_server_addr(port);
    int listenfd = set_up_server_socket(self, 5);

	printf("\033[2J\n"); // clear screen before any logging happens.
    printf("\033[1;35m[ SERVER ]\033[0m \033[1;32minit.c:\033[0m Successfully initialized server on port \033[1;33m%d\033[0m and now awaiting responses.\n", port);
    start_lobby(listenfd);

    if (setup_game() == GAME_SUCCESS){
        printf("\033[1;35m[ SERVER ]\033[0m \033[1;32minit.c:\033[0m Successfully initialized cards.\n");
    }

    if(game_loop(2) == GAME_SUCCESS){
       printf("\033[1;35m[ SERVER ]\033[0m \033[1;32minit.c:\033[0m Game loop returned with GAME_SUCCESS.\n");
    }

    if(determine_game_winner()==GAME_SUCCESS){
         printf("\033[1;35m[ SERVER ]\033[0m \033[1;32minit.c:\033[0m Selected a winner.\n");
    }

    if(send_game_results() == GAME_SUCCESS){
        printf("\033[1;35m[ SERVER ]\033[0m \033[1;32minit.c:\033[0m Sent game results to clients.\n");
    }

    if (wrap_up_game() == GAME_SUCCESS){
        printf("\033[1;35m[ SERVER ]\033[0m \033[1;32minit.c:\033[0m Game finished, beginning cleanup processes.\n");
    }

    free(self);
    printf("\033[1;35m[ SERVER ]\033[0m \033[1;32minit.c:\033[0m Cleanup processes finished, terminating myself now.\n");
    return 0;
}
