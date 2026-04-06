#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "gamestructs.h"
#include "client_output.h"
#include "client_ui_lobby.h"
#include "client_ui_prompt.h"
#include "client_ui_vote.h"
#include "output.h"

volatile int terminal_width = 0; // force the program to read the latest value instead of a cached one

void update_size(int sig) {
	(void)sig;
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
	for (int i = 0; i < n; i++) { printf("%s", to_disp); }
}

void justify_text_format(char* text_format, int min_char, int max_char, char* text) {
	if (text == NULL || text_format == NULL || min_char <= 0 || max_char <= 0) { // safety
		return;
	}

	int width = min_char;
	int len = strlen(text);
	int pos = 0;
	char line[width + 1];   // we could malloc but apparently we can define variable length arrays so why not

	while (pos < len) {
		while (pos < len && text[pos] == ' ') { // skip leading spaces for the next line
			pos++;
		}
		if (pos >= len) { // if we reached the end of the text after skipping spaces, break out of the loop
			break;
		}
		// pos is now at the start of a word

		int remaining = len - pos;
		if (remaining <= width) {
			memcpy(line, text + pos, remaining); // shift the text to next line
			line[remaining] = '\0';              // make sure to null terminate the line
			printf(text_format, min_char, max_char, line);
			break;
		}
		// very easy case where the rest of the text fits within the width, just print it and break

		int window_end = pos + width;
		int break_pos = -1;
		for (int i = pos; i < window_end; i++) {
			if (text[i] == ' ') {
				break_pos = i;
			}
		}
		// find the last space within the width to break on if there is one. if there isn't one, break_pos remains at -1

		if (break_pos != -1) {  // there is space for a well printed line
			int chunk_len = break_pos - pos;
			if (chunk_len > width) { // safety check, should never happen since break_pos is guaranteed to be within the width
				chunk_len = width;
			}
			memcpy(line, text + pos, chunk_len); // shift the text to the next line
			line[chunk_len] = '\0';
			printf(text_format, min_char, max_char, line);
			pos = break_pos + 1; // move past the space for the next line
		} else { // there word is bigger than the width! we have to break in the middle of the word, so just print as much as we can and then move on to the next part of the word
			if (width == 1) { // this is a weird edge case where we can only print one char at a time, so just print the next char and move on
				line[0] = text[pos];
				line[1] = '\0';
				printf(text_format, min_char, max_char, line);
				pos++;
			} else { // okay, now we just break the word as normal, but we add a dash at the end to indicate that its broken in the middle. this is a pretty standard way to break words so it should be fine.
				int chunk_len = width - 1;
				memcpy(line, text + pos, chunk_len); // once again, shift the text to the next line
				line[chunk_len] = '-'; // add a dash to indicate that the word is being broken up
				line[chunk_len + 1] = '\0';         // make sure to null terminate the line
				printf(text_format, min_char, max_char, line);
				pos += chunk_len; // move forward by the chunk length, but we don't add one for the dash because the dash is replacing a character in the word, not adding an extra character
			}
		}
	}
}

void center_text_display(char* text) {
    int left_pad = (terminal_width - strlen(text)) / 2;
    display_n_times(" ", left_pad);
    printf("%s", text);
    display_n_times(" ", terminal_width - strlen(text) - left_pad);
}

void show_card_prompt(Card card) {
	ui_show_card_prompt(card);
}

char* get_card_prompt_response(Card card, int max_chars) {
	return ui_collect_card_response(card, max_chars);
}

void show_vote_card(Card card, int response_count) {
	ui_show_vote_card(card, response_count);
}

void show_results_card(Card card) {
	ui_show_results_card(card);
}

int init_display() {
	signal(SIGWINCH, update_size); // automatically update terminal_width when terminal is resized
	
	update_size(0); // get first terminal size
	
	display_title();

	return 0;
}

void clear_screen() {
	printf("\033[2J\033[H\n");
}

void server_select(char* name, char* port, char* addr) {
	ui_server_select(name, port, addr);
}
