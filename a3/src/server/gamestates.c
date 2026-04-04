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
Card **cards;
Card * drawn_card;
// all logic of intializing the game should go here 
response setup_game() {
    cards = generate_cards();
    return GAME_SUCCESS; 
}

// start the round of the game
response play_round() {
    drawn_card = draw_random(cards);
    if(drawn_card != NULL){
        return GAME_SUCCESS; 
    }
    return GAME_FAIL;
}

response await_responses() {
    drawn_card->responses = malloc(sizeof(Response *)*LOBBY_SIZE);
    for(int i = 0; i < LOBBY_SIZE; i++){
        drawn_card->responses[i] = malloc(sizeof(Response)); //allocate space to be filled later
        drawn_card->responses[i]->player = &players[i]; // iterate through player array 
        printf("player pid: %d \n",drawn_card->responses[i]->player->p_id);
        drawn_card->responses[i]->response = NULL; 
    }
    
    if(s_send(ctop(*(drawn_card))) == GAME_SUCCESS){
        printf("Sent messages to players \n");
    }

    if(s_listen(60) == TIMEOUT){ /// time limit to check for responses
        printf("responses recorded and timeout \n"); 
        for(int i = 0; i < LOBBY_SIZE; i++){ 
            for(int j = 0; j < LOBBY_SIZE; j++){
                if(drawn_card->responses[j]->player->p_id == players[i].p_id){
                    drawn_card->responses[j]->response = players[i].active.data;
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
    return GAME_SUCCESS; 
}

response wrap_up_game() {
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



    return GAME_SUCCESS; 
}

response determine_round_winner(){
        return GAME_SUCCESS; 
}

response determine_game_winner(){
        return GAME_SUCCESS; 
}
