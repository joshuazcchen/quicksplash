#include "gamestructs.h"
#include "gamestates.h"
#include "cards.h"
#include "comms.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include "server_comms.h"
#include "protocol.h"
extern Player players[LOBBY_SIZE];
extern int PLR_COUNT;
Card **cards;
Card * drawn_card;

// all logic of intializing the game should go here
response setup_game() {
    for(int i = 0; i < PLR_COUNT; i++){
         players[i].round_wins = 0;
    }
    cards = generate_cards();
    return GAME_SUCCESS;
}

// start the round of the game
response play_round() {
    for(int i = 0; i < PLR_COUNT; i++){
         players[i].round_votes = 0;
    }
    drawn_card = draw_random(cards);
    if(drawn_card != NULL){
        return GAME_SUCCESS;
    }
    return GAME_FAIL;
}

response await_responses() {
    printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m There are currently %d players in the Lobby.\n",PLR_COUNT);
    drawn_card->responses = calloc(LOBBY_SIZE, sizeof(Response*));
    for(int i = 0; i < PLR_COUNT; i++){
		if (players[i].fd > 2) {
			drawn_card->responses[i] = malloc(sizeof(Response)); //allocate space to be filled later
			drawn_card->responses[i]->player = &players[i]; // iterate through player array 
			drawn_card->responses[i]->response = NULL; 
		} else {
			drawn_card->responses[i] = NULL;
		}
    }
    
	Packet p = ctop(*(drawn_card)); 
    p.header.type = PKT_CARD;
    if(s_send(&p) == GAME_SUCCESS){
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Sent players prompt.\n");
    }
	free(p.data);

    if(s_listen(60) == TIMEOUT){ // time limit to check for responses
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Recorded all player responses or timed out.\n");
        for(int i = 0; i < PLR_COUNT; i++){
            for(int j = 0; j < PLR_COUNT; j++){
                if(drawn_card->responses[j]->player->p_id == players[i].p_id){

					if (players[i].fd == -1) {
						drawn_card->responses[j]->response = NULL;
                        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Player PID %d named %s has disconnected or never replied.\n", players[i].p_id, players[i].name);
					} else if (players[i].ready) {
						char* p = pkttostr(&(players[i].active));
						if (p != NULL) {
							drawn_card->responses[j]->response = strdup(pkttostr(&(players[i].active)));
							printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Stored response %s for player PID %d in Response array.\n", drawn_card->responses[j]->response,drawn_card->responses[j]->player->p_id);
						} else {

							printf("\033[1;35m[ SERVER ]\033[0m\033[1;31m gamestates.c:\033[0m No response received for player PID %d in Response array.\n", drawn_card->responses[j]->player->p_id);
						}
						players[i].ready = 0;
                        free(players[i].active.data);
					}
                }
                
            }
        }
    }


    // server goes here to receive player resposnses
    return GAME_SUCCESS;
}

//end the round
response end_round() {
    free_card(drawn_card);
    drawn_card = NULL;

	for (int i = 0; i < LOBBY_SIZE; i++) {
		if (players[i].state == READY && players[i].fd == -1) {
            printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Player PID %d with Name %s disconnected.\n",players[i].p_id,players[i].name);

			char leave_msg[40];
			sprintf(leave_msg, "%d %s", players[i].p_id, players[i].name);
			Packet leave_pkt = strtopkt(PKT_PLR_DC, leave_msg);
			s_send(&leave_pkt);
			free(leave_pkt.data);

			players[i].state = DISCONNECTED;
			PLR_COUNT--;
		}
	}
    return GAME_SUCCESS;
}

response wrap_up_game() {
    Packet p = strtopkt(PKT_GAME_END, "GMAE OVER");
    if(s_send(&p) == GAME_SUCCESS){
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Sent packet to terminate clients.\n");
    }
    free(p.data);

     for(int i = 0; i < PLR_COUNT; i++){
         players[i].round_wins = 0;
    }
    free_cards(cards);
    cards = NULL;
    return GAME_SUCCESS;
}

response player_join() {
    return GAME_SUCCESS;
}

response player_leave() {
    return GAME_SUCCESS;
}

response initiate_vote() {
    
	Packet p = ctop(*(drawn_card));
    p.header.type = PKT_VOTE;
    if(s_send(&p) == GAME_SUCCESS){
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Sent players responses to vote on.\n");
    }
	free(p.data);

    if(s_listen(15) == TIMEOUT){ // time limit to check for responses
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Received all votes or timed out, server is ready to tally votes.\n");
        for(int i = 0; i < PLR_COUNT; i++){
			if (players[i].fd == -1 || players[i].ready == 0) {
                printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m No vote from %d\n", players[i].p_id);
				continue;
			}
            printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Checking what player PID %d voted; they voted %d.\n", players[i].p_id, (int)strtol(pkttostr(&players[i].active), NULL, 10));
            for(int j = 0; j < PLR_COUNT; j++){
                // ASSUME THAT WHAT THE PLAYER SENDS BACK IS THE PID OF WHO IT VOTED 
                if(drawn_card->responses[j]->player->p_id == strtol(pkttostr(&players[i].active), NULL, 10)){
                    drawn_card->responses[j]->player->round_votes++; 
                    printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Player PID %d now has %d votes.\n",drawn_card->responses[j]->player->p_id,drawn_card->responses[j]->player->round_votes);
                    free(players[i].active.data);
                }
            }
        }
    }
    
    return GAME_SUCCESS;
}

response determine_round_winner(){
    int max_index = 0; 
    for (int i = 1; i < PLR_COUNT; i++) {
        if (players[i].round_votes > players[max_index].round_votes) {
            max_index = i;
        }
    }
    int tie_count = 0; 
    for(int i = 0; i < PLR_COUNT; i++){
        if(players[max_index].round_votes == players[i].round_votes){
            tie_count++;
        }
    }
    if(tie_count > 1){
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m A Tie has between two or more players.\n");
        for(int i = 0; i < PLR_COUNT;i++){
            if (players[i].round_votes == players[max_index].round_votes) {
                players[i].round_wins += 1;
            }
        }
    }
    else{
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Player PID %s wins the round with %d votes.\n", players[max_index].name,players[max_index].round_votes);
        players[max_index].round_wins += 1;
    }

    return GAME_SUCCESS;
}

response send_round_vote_results() {
    if (drawn_card == NULL || drawn_card->responses == NULL) {
        return GAME_FAIL;
    }

    char payload[8192] = {0};
    int used = 0;

    int max_votes = -1;
    for (int i = 0; i < PLR_COUNT; i++) {
        Response* response = drawn_card->responses[i];
        if (response != NULL && response->player != NULL) {
            if (response->player->round_votes > max_votes) {
                max_votes = response->player->round_votes;
            }
        }
    }

    if (max_votes < 0) {
        max_votes = 0;
    }

    used += snprintf(payload + used, sizeof(payload) - used, "ROUND VOTE RESULTS\n");
    used += snprintf(payload + used, sizeof(payload) - used, "Prompt: %s\n\n", drawn_card->prompt_text ? drawn_card->prompt_text : "[missing prompt]");

    for (int i = 0; i < PLR_COUNT; i++) {
        Response* response = drawn_card->responses[i];
        if (response == NULL || response->player == NULL) {
            continue;
        }

        char* name = (strlen(response->player->name) > 0) ? response->player->name : "Unknown";
        char* text = (response->response != NULL && strlen(response->response) > 0) ? response->response : "[no response]";
        int votes = response->player->round_votes;
        int winner = (votes == max_votes);

        used += snprintf(payload + used, sizeof(payload) - used, "%d) %s%s%s - %d vote%s\n", i + 1,
                         winner ? "* " : "", name, winner ? " (ROUND WINNER)" : "", votes, votes == 1 ? "" : "s");
        used += snprintf(payload + used, sizeof(payload) - used, "   %s\n\n", text);

        if (used >= (int)sizeof(payload) - 64) {
            break;
        }
    }

    Packet p = strtopkt(PKT_STATS, payload);
    if (s_send(&p) == GAME_SUCCESS) {
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Sent round vote results to all clients.\n");
    }
    free(p.data);

    return GAME_SUCCESS;
}

response wait_for_host_continue() {
    if (PLR_COUNT <= 0 || players[0].fd <= 2) { // if no players or host disconnected, fail immediately
        return GAME_FAIL;
    }

    printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Waiting for host to continue to the next round.\n");

    while (1) { // TODO joshua this while(1) might be risky but it should handly all clients correctly so idk
        fd_set fds;
        FD_ZERO(&fds); // we'll be listening to all clients here

        int high = -1;
        for (int i = 0; i < LOBBY_SIZE; i++) {
            if (players[i].fd > 2) {
                FD_SET(players[i].fd, &fds); // add them to the set of fds we're listening to
                if (players[i].fd > high) {
                    high = players[i].fd; // keep track of the highest fd for select
                }
            }
        }

        if (high < 0) { // if no valid fds to listen to, fail immediately (should only happen if all clients disconnected while waiting)
            return GAME_FAIL;
        }

        int listen = select(high + 1, &fds, NULL, NULL, NULL); // block until one of the clients we're listening to sends something
        if (listen <= 0) {
            continue;
        }

        for (int i = 0; i < LOBBY_SIZE; i++) { // check which client sent something
            if (players[i].fd <= 2 || !FD_ISSET(players[i].fd, &fds)) { // if this client isn't valid or didn't send anything, skip it
                continue;
            }

            response ret = s_read(&players[i]); // read the packet they sent and store it in their active packet
            if (ret == CLIENT_DISCONNECT) { // if they disconnected while we were waiting, we need to handle that
                if (i == 0) { // uh oh, it's the host that left!!
                    printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Host disconnected while waiting to continue.\n");
                    return CLIENT_DISCONNECT;
                }

                printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Player PID %d disconnected while waiting for host continue.\n", players[i].p_id);
                close(players[i].fd); // close their socket
                players[i].fd = -1; // mark them as disconnected in our player array
                if (players[i].active.data != NULL) { // free any data in their active packet to prevent memory leaks
                    free(players[i].active.data);
                    players[i].active.data = NULL;
                }
                continue; // we can continue waiting for the host new to send the continue packet, other clients disconnecting shouldn't affect that
            }

            if (ret == READ_SUCCESS && players[i].ready) { // if we got a packet and it's ready to be processed
                if (i == 0 && players[i].active.header.type == PKT_START) { // if the host sent the continue packet, we can break out of the loop and start the next round
                    if (players[i].active.data != NULL) { // make sure to free the active packet data of the host to prevent memory leaks
                        free(players[i].active.data);
                        players[i].active.data = NULL;
                    }
                    players[i].ready = 0; // reset their ready state for the next round
                    printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Host continued to the next round.\n");
                    return GAME_SUCCESS;
                }

                if (players[i].active.data != NULL) { // same here, if a non-host client sent a packet while we were waiting, we should free it to prevent memory leaks and we won't be doing anything with it
                    free(players[i].active.data);
                    players[i].active.data = NULL;
                }
                players[i].ready = 0;
            }
        }
    }
}

response determine_game_winner(){
    int max_index = 0; 
    for (int i = 1; i < PLR_COUNT; i++) {
        if (players[i].round_wins > players[max_index].round_wins) {
            max_index = i;
        }
    }
    int tie_count = 0; 
    for(int i = 0; i < PLR_COUNT; i++){
        if(players[max_index].round_wins == players[i].round_wins){
            tie_count++;
        }
    }
    if(tie_count > 1){
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m A Tie has between two or more players.\n");
        for(int i = 0; i < PLR_COUNT;i++){
            if (players[i].round_wins == players[max_index].round_wins) {
                printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m %s wins the game with %d points.\n",players[i].name ,players[i].round_wins);
            }
        }
    }
    else{
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m %s wins the game with %d points.\n",players[max_index].name ,players[max_index].round_wins);
    }
    
   
    return GAME_SUCCESS;
}
