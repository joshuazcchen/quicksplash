#include "../../include/gamestructs.h"
#include "../common/cards.c"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//TODO GET RID OF FILEPATHS EVENTUALLY 

Card ** cards;
Card *drawn_card;

// all logic of intializing the game should go here 
void setup_game(){
    cards = generate_cards();
    return; 
}

// start the round of the game
void play_round(){
    drawn_card = draw_random(cards);
    drawn_card->responses = malloc(sizeof(Response *)*LOBBY_SIZE);
    for(int i = 0; i < LOBBY_SIZE; i++){
        drawn_card->responses[i] = malloc(sizeof(Response)); //allocate space to be filled later
        // drawn_card->responses[i]->player; // iterate through player array 
        drawn_card->responses[i]->response = NULL; 
        // printf("drawn card: %s \n player responses: %s \n",drawn_card->prompt_text,drawn_card->responses);
    }

    return; 
}
//end the round 
void end_round(){
    free_card(drawn_card); 
    drawn_card = NULL; 
    return; 
}

void wrap_up_game(){
    free_cards(cards);
    cards = NULL; 
    return; 
}

void player_join(){
    return; 
}

void player_leave(){
    return; 
}

void intitiate_vote(){
    return; 
}

void game_loop(){
    while (1){
        return;
    }

    return; 
}


//TEMP main only used for debugging 
int main(){

    setup_game();
    play_round();
    end_round();
    wrap_up_game();
    return 1; 
}