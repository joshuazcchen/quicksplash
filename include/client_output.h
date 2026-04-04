#ifndef CLIENT_OUTPUT_H
#define CLIENT_OUTPUT_H

#include "cards.h"

extern volatile int terminal_width;

void update_size(int sig);
void display_title();
void display_n_times(char* to_disp, int n);
void justify_text_format(char* text_format, int min_char, int max_char, char* text);
void center_text_display(char* text);
void show_card_prompt(Card card);
int init_display();
void clear_screen();
void server_select(char* name, char* port, char* addr);
void set_default_guest_username(char* name, int name_cap);
void box_print_border(char* start_pad, char* color,\
					  char* left, char* hchar, char* right,\
					  int inner_width);

#endif

