#include <stdio.h>
#include <string.h>
#include "socket.h"
#include "comms.h"
#include "gamestructs.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
    int s = connect_to_server(30000, "127.0.0.1");
    printf("success join");
    while (1) {
        sleep(1);
    }
    return 0;
}

