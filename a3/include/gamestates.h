#ifndef GAMESTATES_H
#define GAMESTATES_H
#include "gamestructs.h"
#include "comms.h"

response setup_game();
response play_round();
response end_round();
response wrap_up_game();
response player_join();
response player_leave();
response initiate_vote();
response game_loop();

#endif
