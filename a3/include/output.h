#ifndef OUTPUT_H
#define OUTPUT_H

extern volatile int terminal_width;

void update_size(int sig);
void display_title();
void show_card_prompt(Card card);
int init_display();

#endif

