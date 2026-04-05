#include "gamestructs.h"
#include "gamestates.h"
#include "cards.h"
#include "comms.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
    printf("await_responses: There are currently %d players \n",PLR_COUNT);

    drawn_card->responses = malloc(sizeof(Response *)*PLR_COUNT);
    for(int i = 0; i < PLR_COUNT; i++){
		if (players[i].fd > 2) {
			drawn_card->responses[i] = malloc(sizeof(Response)); //allocate space to be filled later
			drawn_card->responses[i]->player = &players[i]; // iterate through player array 
			printf("initialized player response for PID: %d \n",drawn_card->responses[i]->player->p_id);
			drawn_card->responses[i]->response = NULL; 
		} else {
			drawn_card->responses[i] = NULL;
		}
    }
    
	Packet p = ctop(*(drawn_card)); 
    p.header.type = PKT_CARD;
    if(s_send(&p) == GAME_SUCCESS){
        printf("Sent Prompt To Players\n");
    }
	free(p.data);

    if(s_listen(60) == TIMEOUT){ // time limit to check for responses
        printf("responses recorded and timeout \n");
        for(int i = 0; i < PLR_COUNT; i++){
            for(int j = 0; j < PLR_COUNT; j++){
                if(drawn_card->responses[j]->player->p_id == players[i].p_id){

					if (players[i].fd == -1) {
						drawn_card->responses[j]->response = NULL;
						printf("here disconnect player die %d %s\n", players[i].p_id, players[i].name);
					} else if (players[i].ready) {
						drawn_card->responses[j]->response = strdup(pkttostr(&(players[i].active)));
						printf("await_responses: found pid %d, with response recorded as %s \n",drawn_card->responses[j]->player->p_id, drawn_card->responses[j]->response);
						players[i].ready = 0;
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
			printf("this player disconnected midgame so die\n");
			// TODO: free additional relevant data from them.
			//
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
        printf("Sent votecount to Players \n");
    }
	free(p.data);

    if(s_listen(15) == TIMEOUT){ // time limit to check for responses
        printf("recorded votes and is now tallying the votes\n");
        for(int i = 0; i < PLR_COUNT; i++){
			if (players[i].fd == -1 || players[i].ready == 0) {
				printf("no vote from %d\n", players[i].p_id);
				continue;
			}
            printf("checking what player pid %d voted, they voted %d \n", players[i].p_id, (int)strtol(pkttostr(&players[i].active), NULL, 10));
            for(int j = 0; j < PLR_COUNT; j++){
                // ASSUME THAT WHAT THE PLAYER SENDS BACK IS THE PID OF WHO IT VOTED 
                if(drawn_card->responses[j]->player->p_id == strtol(pkttostr(&players[i].active), NULL, 10)){
                    drawn_card->responses[j]->player->round_votes++; 
                    printf("Player %d now has %d votes \n",drawn_card->responses[j]->player->p_id,drawn_card->responses[j]->player->round_votes);
                }
            }
        }
    }
    return GAME_SUCCESS;
}

response determine_round_winner(){
    int max_index = 0; 
    for(int i = 1; i < PLR_COUNT; i++){
        if(players[i-1].round_votes <= players[i].round_votes){
            max_index = i;
        }
    }
    printf("PLAYER: %d WINS ROUND!",players[max_index].p_id);
    players[max_index].round_wins += 1;
    return GAME_SUCCESS;
}

response determine_game_winner(){
    int max_index = 0; 
    for(int i = 1; i < PLR_COUNT; i++){
        if(players[i-1].round_wins <= players[i].round_wins){
            max_index = i;
        }
    }
    printf("PLAYER: %d WINS GAME!",players[max_index].p_id);
    return GAME_SUCCESS;
}
