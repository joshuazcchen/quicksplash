#include "gamestructs.h"
#include "cards.c"
#include "comms.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Card ** cards;
Card *drawn_card;

// all logic of intializing the game should go here 
response setup_game(){
    cards = generate_cards();
    return GAME_SUCCESS; 
}

// start the round of the game
response play_round(){
    drawn_card = draw_random(cards);
    drawn_card->responses = malloc(sizeof(Response *)*LOBBY_SIZE);
    for(int i = 0; i < LOBBY_SIZE; i++){
        drawn_card->responses[i] = malloc(sizeof(Response)); //allocate space to be filled later
        // drawn_card->responses[i]->player; // iterate through player array 
        drawn_card->responses[i]->response = NULL; 
        // printf("drawn card: %s \n player responses: %s \n",drawn_card->prompt_text,drawn_card->responses);
    }

    return GAME_SUCCESS; 
}

//end the round 
response end_round(){
    free_card(drawn_card); 
    drawn_card = NULL; 
    return GAME_SUCCESS; 
}

response wrap_up_game(){
    free_cards(cards);
    cards = NULL; 
    return GAME_SUCCESS; 
}

response player_join(){
    return GAME_SUCCESS; 
}

response player_leave(){
    return GAME_SUCCESS; 
}

response intitiate_vote(){
    return GAME_SUCCESS; 
}

response game_loop(){
    return GAME_SUCCESS; 
}
