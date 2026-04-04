#ifndef OUTPUT_H
#define OUTPUT_H

extern volatile int terminal_width;

void update_size(int sig);
void display_title();
void display_n_times(char* to_disp, int n);
void justify_text_format(char* text_format, int min_char, int max_char, char* text);
void show_card_prompt(Card card);
int init_display();

void clear_screen();
void server_select(char* name, char* port, char* addr);

#endif

