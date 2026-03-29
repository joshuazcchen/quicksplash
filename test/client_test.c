#include <stdio.h>
#include <string.h>
#include "socket.h"
#include "comms.h"
#include "gamestructs.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
    // must have only one extra param
    int s = connect_to_server(30000, "127.0.0.1");
    Packet p1;
    p1.type = 1;
    strcpy(p1.data, "penis\r\n");
    write(s, &p1, sizeof(Packet));

    printf("spaghetti");
    p1.type = 2;
    strcpy(p1.data, "aaaaadsf\r\n");
    write(s, &p1, sizeof(Packet));

    strcpy(p1.data, "orange\r\n");
    write(s, &p1, sizeof(Packet));

    close(s);
    return 0;
}

