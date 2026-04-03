#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h> 
#include "gamestructs.h"
#include "gamestates.h"
#include "comms.h"


// code only for testing 
int main(){

    if (setup_game() == GAME_SUCCESS){
        printf("CARDS INITIALIZED \n");
    }
    
    if(game_loop(5) == GAME_SUCCESS){
        printf("GOOD! \n");
    }

    if (wrap_up_game() == GAME_SUCCESS){
        printf("GAME ENDED WOWWWWWWWWWWWWWWWWWWWWWWWWW\n");
    }


    return 1; 
}
