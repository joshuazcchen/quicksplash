#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "gamestructs.h"
#include "client_output.h"
#include "client_input.h"
#include "output.h"

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
	for (int i = 0; i < n; i++) { printf("%s", to_disp); }
}

void justify_text_format(char* text_format, int min_char, int max_char, char* text) {
	int i = 0;
	int len = strlen(text);
	do {
		printf(text_format, min_char, max_char, text + i);
		i += min_char;
	} while (i < len);
}

void center_text_display(char* text) {
    int left_pad = (terminal_width - strlen(text)) / 2;
    display_n_times(" ", left_pad);
    printf("%s", text);
    display_n_times(" ", terminal_width - strlen(text) - left_pad);
}

void show_card_prompt(Card card) {
	int horizontal_size = terminal_width / 2;
	int j = (terminal_width - horizontal_size) / 2;
	char start_pad[j + 1];
	memset(start_pad, ' ', j);
	start_pad[j] = '\0';

	printf("%s╭", start_pad);
	display_n_times("─", horizontal_size - 2);
	printf("╮\n");

	char* title = "NEW CARD";
	int left_pad = (horizontal_size - 2 - strlen(title)) / 2;
	int right_pad = (horizontal_size - 2 - strlen(title)) - left_pad;
	printf("%s│", start_pad);
	display_n_times(" ", left_pad);
	printf("%s", title);
	display_n_times(" ", right_pad);
	printf("│\n");

	char* desc = "Type a funny response to this sentence:";
	char format[strlen(start_pad) + 10 + 1];
	strcpy(format, start_pad);
	strcat(format, "│%-*.*s│\n");
	justify_text_format(format, horizontal_size-2, horizontal_size-2, desc);
	justify_text_format(format, horizontal_size-2, horizontal_size-2, card.prompt_text);

	printf("%s╰", start_pad);
	display_n_times("─", horizontal_size-2);
	printf("╯\n");
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
	printf("\n\033[44m\033[1;37m"); // blue with white text
	display_n_times(" ", terminal_width);
	printf("\n");
	if (terminal_width > 12) {
		center_text_display("join a lobby");
	} else {
		printf("join a lobby\n");
	}
	display_n_times(" ", terminal_width);
	printf("\033[0m\n"); // reset
	printf("\033[1;32m"); // i dont know colour theory man, green ig
	if (terminal_width > 12) {
	    center_text_display("user");	
		printf("\n\n");
		// display_n_times(" ", terminal_width);
	} else {
		printf("user\n");
	}
    int input_offset = terminal_width / 2 - 6;
	display_n_times(" ", input_offset);
	printf("> ");
    get_str_to_ptr(name, 32);

	printf("\033[0m\n"); // reset
	printf("\033[1;32m"); // i dont know colour theory man, green ig
	if (terminal_width > 12) {
        center_text_display("port");
		printf("\n\n");
		// display_n_times(" ", (int)(terminal_width - 12) / 2);
	} else {
		printf("port\n");
	}
	display_n_times(" ", input_offset);
	printf("> ");
    get_str_to_ptr(port, 7);

	printf("\033[0m\n"); // reset
	printf("\033[1;32m"); // i dont know colour theory man, green ig
	if (terminal_width > 12) {
        center_text_display("server");
		printf("\n\n");
		// display_n_times(" ", (int)(terminal_width - 12) / 2);
	} else {
		printf("server\n");
    }
	display_n_times(" ", input_offset);
	printf("> ");
    get_str_to_ptr(addr, 30);

    // Default replacements
    if (strlen(port) == 0) strcpy(port, "30000");
    if (strlen(addr) == 0) strcpy(addr, "127.0.0.1");
	printf("\033[1;32m");
	printf("\n\033[44m\033[1;37m"); // blue with white text
	display_n_times(" ", terminal_width);
	printf("\n");
	if (terminal_width > 16) {
        center_text_display("joining lobby");
	} else {
		printf("joining lobby\n");
	}
	display_n_times(" ", terminal_width);
    printf("\n\n");
    char buf[sizeof("Connecting to : as ") + 30 + 7 + 32];
    sprintf(buf, "Connecting to %s:%s as %s", addr, port, name);
    center_text_display(buf);
    printf("\033[0m\n");
}

