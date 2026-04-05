#ifndef GAMESTRUCTS_H
#define GAMESTRUCTS_H
#define MAX_PROMPT_SIZE 1000
#define MAX_RESPONSE_SIZE 256
#define PROMPT_COUNT 135
#define LOBBY_SIZE 5
#include <stdint.h>

typedef enum {
    P_TEST = 1,
    P_JOIN = 2,
    P_LEAVE = 3,
    P_CARD = 4,
    P_REPLY = 5,
    P_VOTE = 6,
    P_STATS = 7,
    P_START = -3
} p_type;

typedef enum {
	PKT_TEST = 1,
	PKT_JOIN = 2,
	PKT_LEAVE = 3,
	PKT_CARD = 4,
	PKT_REPLY = 5,
	PKT_VOTE = 6,
	PKT_STATS = 7,
	PKT_START = 0,
	PKT_QUIT = -1, // client disconnect
	PKT_PLR_DC = -1,
	PKT_GAME_END = -2 // game end
} pkt_type; // REDOING BECAUSE WE HAVE TOO MANY THINGS NAMED P_ LOL. PACKETS ARE NOW PKT

typedef struct {
	pkt_type type;
	// also this could probably just end up being subbed out for a short but if this isnt actually truly 16 bits then everything else will absolutely vapourize itself. MIGHT AS WELL IMPORT SMTH JUST TO BE SAFE LOL
	uint16_t length; // maximum of 65k ish but if we ever need more length, we can always update this accordingly.
					 // moreso like this just so if we ever end up with a faulty client they can only ever really freeze server for 65k bytes at a given time (65kb  is basically instant lol).
} PacketHeader;

typedef struct {
	PacketHeader header;
	char *data; // this is gonna have to be dynamically allocated as opposed to the fixed buffersize from before.
} Packet;

typedef enum {
	HEADER = 1, 
	PAYLOAD = 2
} pkt_state; // state of the current read, whether we're awaiting the next header or the full payload of the packet

typedef enum {
    DISCONNECTED = 0,
    PENDING = 1,
    READY = 2
} p_state;

typedef struct player { // have to have the player alias at the top too so we can self reference
    int p_id; // we can store the user's machine info somewhere inside this struct too so that we know who we're listening to from the serverside?
    int fd;
    char name[32];
	p_state state; // this is player state not packet state, stupid naming.
    Packet active;
	pkt_state c_state; // stupid variable name just determines whether we already received a header
	int h_inbuf; // header inbuf
	int p_inbuf; // actual packet/payload inbuf
	int ready;
    int round_votes; 
    int round_wins;
} Player;

typedef struct{
    Player* player; // player sending the response 
    char* response; // the response player typed in
} Response;


typedef struct{
    char* prompt_text; //prompt_text of the card
    Response** responses; //array of player responses
} Card;
#endif

