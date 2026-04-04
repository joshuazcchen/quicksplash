#ifndef GAMESTATES_H
#define GAMESTATES_H
#include "gamestructs.h"
#include "comms.h"

extern Card **cards;
extern Card * drawn_card;;

response setup_game();
response play_round();
response await_responses();
response end_round();
response wrap_up_game();
response player_join();
response player_leave();
response initiate_vote();
response game_loop();
response determine_round_winner();
response determine_game_winner();

#endif

