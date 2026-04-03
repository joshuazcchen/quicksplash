#include <stdio.h>
#include "gamestructs.h"

void display_title() {
  printf("WELCOME TO QUICK SPLASH !!!");
}

void show_card_prompt(Card card) {
  printf("╭────────────────────────────────────╮\n\
│             NEW CARD               │\n\
│Type a funny response to this       │\n\
│sentence:                           ");
  int horizontal_size = 36;
  int i = 0;
  for (;(card.prompt_text)[i] != '\0'; i++) {
    if (i % horizontal_size == 0) {
      printf("│\n│");
    }
    printf("%c", (card.prompt_text)[i]);
  }
  while (i % horizontal_size != 0) {
    printf(" ");
    i++;
  }
  printf("│\n");
  printf("╰────────────────────────────────────╯");
}

