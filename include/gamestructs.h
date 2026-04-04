#ifndef GAMESTRUCTS_H
#define GAMESTRUCTS_H
#define BUFFERSIZE 256
#define buffersize 256 // TODO: why did i use two different ones???????????? i forgot that the header included this 
#define MAX_PROMPT_SIZE 1000
#define MAX_RESPONSE_SIZE 256
#define PROMPT_COUNT 135
#define LOBBY_SIZE 5

typedef enum {
    P_TEST = 1,
    P_JOIN = 2,
    P_LEAVE = 3,
    P_CARD = 4,
    P_REPLY = 5,
    P_VOTE = 6,
    P_STATS = 7,
    P_START = 8
} p_type;

typedef enum {
    DISCONNECTED = 0,
    PENDING = 1,
    READY = 2
} p_state;

typedef struct {
    p_type type;
    char data[BUFFERSIZE];
} Packet; // data sent to and from the client, declare a specific id for type of info so it can be processed accordingly.

typedef struct {
    int p_id; // we can store the user's machine info somewhere inside this struct too so that we know who we're listening to from the serverside?
    int fd;
    char name[32];

    // idk how else to do this so ig we got a staging area now. getting closer to accidentally making git by the minute
    Packet active;
    int ready;
    Packet partial;
    int inbuf;
    p_state state;

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

