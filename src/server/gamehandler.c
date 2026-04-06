#include "gamestates.h"
#include "gamestructs.h"
#include <stdio.h>
#include <string.h>



// all server-client communication goes on here


response game_loop(int round_count){
    int rounds = round_count;
    // start round
    while(rounds > 0){
        printf("\n");
        if(play_round() == GAME_SUCCESS){
            printf("\033[1;35m[ SERVER ]\033[0m\033[1;32m gamestates.c:\033[0m Round setup, starting game.\n");
        }
        // send round prompt to players
        // wait for all player responses
        if(await_responses()== GAME_SUCCESS){
            printf("\033[1;35m[ SERVER ]\033[0m\033[1;32m gamestates.c:\033[0m Prompts sent, and have been recorded.\n");
        }
        // initiate voting
        if (initiate_vote() == GAME_SUCCESS){
            printf("\033[1;35m[ SERVER ]\033[0m\033[1;32m gamestates.c:\033[0m Voting options sent, and have been recorded.\n");
        }
        // determine winner or round
        if (determine_round_winner() == GAME_SUCCESS){
            printf("\033[1;35m[ SERVER ]\033[0m\033[1;32m gamestates.c:\033[0m A winner has been determined.\n");
        }
        if (send_round_vote_results() == GAME_SUCCESS) {
            printf("\033[1;35m[ SERVER ]\033[0m\033[1;32m gamestates.c:\033[0m Round results sent to all players.\n");
        }
        if (wait_for_host_continue() == GAME_SUCCESS) {
            printf("\033[1;35m[ SERVER ]\033[0m\033[1;32m gamestates.c:\033[0m Host approved the next round.\n");
        }
        // end round
        if(end_round() == GAME_SUCCESS){
            printf("\033[1;35m[ SERVER ]\033[0m\033[1;32m gamestates.c:\033[0m Round has sucessfully ended\n");
        }
        // repeat if game isnt over
        rounds--;
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;32m gamestates.c:\033[0m There is currently %d rounds left.\n",rounds);

    }
    return GAME_SUCCESS;
}

