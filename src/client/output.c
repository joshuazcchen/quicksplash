#include <stdio.h>
#include "gamestructs.h"

void display_title() {
  printf("WELCOME TO QUICK SPLASH !!!");
}

void show_card_prompt(Card card) {
  printf("╭────────────────────────────────────╮\n
          │             NEW CARD               │
          │Type a funny response to this       │
          │sentence:                           │");
  horizontal_size = 36;
  char to_print[horizontal_size + 1]; // +1 for \0
  int i = 0;
  for (i; card.prompt_text[i] != '\0'; i++) {
    if (i % horizontal_size == 0) {
      printf("│\n│");
    }
    printf("%s", card.prompt_text[i]);
  }
  while (i % horizontal_size != 0) {
    printf(" ");
  }
  printf("│\n");
}

