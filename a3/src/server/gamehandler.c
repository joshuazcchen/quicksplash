#include "gamestates.h"
#include "gamestructs.h"
#include <stdio.h>
#include <string.h>



// all server-client communication goes on here 


response game_loop(int round_count){
    int rounds = round_count;
    // start round 
    while(rounds > 0){
        if(play_round() == GAME_SUCCESS){
            printf("SUCESS \n");
        }
        // send round prompt to players
        // wait for all player responses 
        if(await_responses()== GAME_SUCCESS){
            printf("RESPONSES READY TO SEND \n");
        }
        // initiate voting 
        if (initiate_vote() == GAME_SUCCESS){
            printf("vote starting \n");
        }
        // determine winner or round
        if (determine_round_winner() == GAME_SUCCESS){
            printf("WE HAVE A WINNER \n");
        }
        // end round 
        if(end_round() == GAME_SUCCESS){
            printf("ROUND ENDED \n");
        }
        // repeat if game isnt over 
        rounds--; 
        printf("rounds left: %d \n", rounds);
    }
    return GAME_SUCCESS;
}   