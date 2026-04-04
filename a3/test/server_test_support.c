#include "gamestructs.h"
#include "comms.h"

Player players[LOBBY_SIZE];

// test doesn't need real implementations of these
response s_send(Packet p) {
	(void)p;
	return GAME_SUCCESS;
}

// same here lol
response s_listen(int max_time) {
	(void)max_time;
	return GAME_SUCCESS;
}
