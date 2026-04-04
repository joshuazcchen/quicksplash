#include <stdio.h>
#include <string.h>
#include "socket.h"
#include "comms.h"
#include "gamestructs.h"
#include <unistd.h>
#include "output.h"
#include <stdlib.h>
#include "client_input.h"

int main(int argc, char *argv[]) {
    if (init_display() != 0) {
      fprintf(stderr, "Error while initializing display");
      exit(1);
    }
    Card a;
    a.prompt_text = malloc(sizeof(char) * 100);
    char test_string[] = "test phrase orange lemon village orangutan";
    strncpy(a.prompt_text, test_string, 99);
    (a.prompt_text)[99] = '\0';
    printf("%s\n", a.prompt_text);
    show_card_prompt(a);

    while (1) {
      char* response;
      response = get_text_input("I need input", 10);
      printf("%s\n", response);
      free(response);
    }

    // printf("Starting to print widths\n");
    // while (1) {
    //   printf("%d\n", terminal_width);
    //   sleep(1);
    // }

    // int s = connect_to_server(30000, "127.0.0.1");
    printf("success join");
    while (1) {
        sleep(1);
    }
    return 0;
}
