#define BUFFERSIZE 256

typedef struct {
    int p_id; // we can store the user's machine info somewhere inside this struct too so that we know who we're listening to from the serverside?
    char name[32];
} Player;

typedef struct {
    int state;
} State; // State of the game, will be sent to the individual clients to change their status of UI? do we necessarily need this or should we just consolidate this into packets.

typedef struct {
    int type;
    char data[BUFFERSIZE];
} Packet; // data sent to and from the client, declare a specific id for type of info so it can be processed accordingly.
