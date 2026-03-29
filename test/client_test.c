#include <stdio.h>
#include <string.h>
#include "socket.h"
#include "comms.h"
#include "gamestructs.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
    // must have only one extra param
    int s = connect_to_server(30000, "127.0.0.1");
    Packet p;
    p.type = 1;
    strncpy(p.data, argv[1], BUFFERSIZE);

    write(s, &p, sizeof(Packet));
    sleep(2);
    close(s);
    return 0;
}
