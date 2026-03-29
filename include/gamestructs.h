#ifndef GAMESTRUCTS_H
#define GAMESTRUCTS_H
#define BUFFERSIZE 256
#define buffersize 256 // TODO: why did i use two different ones???????????? i forgot that the header included this 

typedef struct {
    int type; // 1 is currently being used for testing, we can discuss hwo to do this properly maybe with enums later on
    char data[BUFFERSIZE];
} Packet; // data sent to and from the client, declare a specific id for type of info so it can be processed accordingly.
          
typedef struct {
    int p_id; // we can store the user's machine info somewhere inside this struct too so that we know who we're listening to from the serverside?
    int fd;
    char name[32];
    
    // idk how else to do this so ig we got a staging area now. getting closer to accidentally making git by the minute
    int ready;
    Packet partial;
    int inbuf;
} Player;

#endif
