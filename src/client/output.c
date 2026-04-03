#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "gamestructs.h"
#include "client_output.h"

volatile int terminal_width = 0; // force the program to read the latest value instead of a cached one

void update_size(int sig) {
  struct winsize w;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
    terminal_width = w.ws_col;
  } else {
    perror("ioctl");
  }
}

void display_title() {
  printf("WELCOME TO QUICK SPLASH !!!\n");
}

void show_card_prompt(Card card) {
  printf("╭────────────────────────────────────╮\n\
│             NEW CARD               │\n\
│Type a funny response to this       │\n\
│sentence:                           │\n");
  int horizontal_size = 36;
  int i = 0;
  int len = strlen(card.prompt_text);

  do {
    printf("│%-36.36s│\n", card.prompt_text + i);
    i += 36;
  } while (i < len);

  printf("╰────────────────────────────────────╯\n");
}

int init_display() {
  signal(SIGWINCH, update_size); // automatically update terminal_width when terminal is resized
  
  update_size(0); // get first terminal size
  
  display_title();

  return 0;
}
