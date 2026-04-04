#ifndef SERVER_COMS_H
#define SERVER_COMS_H
#include "gamestructs.h"
#include "comms.h"

response s_send(Packet p);
response s_read(Player *p);
response s_listen(int max_time);

#endif
