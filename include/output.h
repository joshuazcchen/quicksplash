#ifndef OUTPUT_H
#define OUTPUT_H

#include "gamestructs.h"

extern volatile int terminal_width;

void update_size(int sig);
void display_title();
void display_n_times(char* to_disp, int n);
void justify_text_format(char* text_format, int min_char, int max_char, char* text);
void center_text_display(char* text);
void show_card_prompt(Card card);
char* get_card_prompt_response(Card card, int max_chars);
void show_vote_card(Card card, int response_count);
int init_display();

void clear_screen();
void server_select(char* name, char* port, char* addr);

#endif
