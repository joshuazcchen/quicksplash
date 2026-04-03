#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "gamestructs.h"

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

void display_n_times(char* to_disp, int n) {
  for (int i = 0; i < n; i++) { printf(to_disp); }
}

void justify_text_format(char* text_format, int min_char, int max_char, char* text) {
  int i = 0;
  int len = strlen(desc);
  do {
    printf("│%-*.*s│\n", horizontal_size, horizontal_size, desc + i);
    i += horizontal_size;
  } while (i < len);
}

void show_card_prompt(Card card) {
  int horizontal_size = 36;
  printf("╭");
  display_n_times("─", horizontal_size);
  printf("╮");

  char* title = "NEW CARD";
  int left_pad = (horizontal_size - strlen(title)) / 2;
  int right_pad = (horizontal_size - strlen(title)) - left_pad;
  printf("│");
  display_n_times(" ", left_pad);
  printf(title);
  display_n_times(" ", right_pad);
  printf("│\n");

  char* desc = "Type a funny response to this sentence:";
  justify_text_format("│%-*.*s│\n", horizontal_size, horizontal_size, desc);
  justify_text_format("│%-*.*s│\n", horizontal_size, horizontal_size, card.prompt_text);

  printf("╰");
  display_n_times("─", horizontal_size);
  printf("╯\n");
}

int init_display() {
  signal(SIGWINCH, update_size); // automatically update terminal_width when terminal is resized
  
  update_size(0); // get first terminal size
  
  display_title();

  return 0;
}
