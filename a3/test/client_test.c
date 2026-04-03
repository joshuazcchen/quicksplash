#include <stdio.h>
#include <string.h>
#include "socket.h"
#include "comms.h"
#include "gamestructs.h"
#include <unistd.h>
#include "output.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (init_display() != 0) {
      fprintf(stderr, "Error while initializing display");
      exit(1);
    }
    Card a;
    a.prompt_text = malloc(sizeof(char) * 100);
    char test_string[] = "wow thia is a funny card promp, i'm just testing if it works or not by the way...";
    strncpy(a.prompt_text, test_string, 99);
    (a.prompt_text)[99] = '\0';
    printf("%s\n", a.prompt_text);
    show_card_prompt(a);

    printf("Starting to print widths");
    while (1) {
      printf("%d", terminal_width);
      sleep(1);
    }

    // int s = connect_to_server(30000, "127.0.0.1");
    printf("success join");
    while (1) {
        sleep(1);
    }
    return 0;
}

