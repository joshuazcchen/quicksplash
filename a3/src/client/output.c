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
	const char* reset = "\033[0m";
	const char* accent = "\033[1;36m";
	const char* soft = "\033[2;37m";
	const char* strong = "\033[1;97m";

	int width = terminal_width;
	if (width <= 0) width = 80;

	int panel_width;
	if (width >= 72) {
		panel_width = 68;
	} else if (width >= 40) {
		panel_width = width - 4;
	} else {
		panel_width = width;
	}

	int left_pad = (width - panel_width) / 2;
	if (left_pad < 0) left_pad = 0;

	char start_pad[left_pad + 1];
	memset(start_pad, ' ', left_pad);
	start_pad[left_pad] = '\0';

	clear_screen();

	if (panel_width >= 30) {
		int inner_width = panel_width - 2;
		const char* info = "  Fill in the fields. Press Enter to use defaults.";
		printf("%s%s╭", start_pad, accent);
		display_n_times("─", panel_width - 2);
		printf("╮%s\n", reset);
		printf("%s%s│  QUICK SPLASH: LOBBY CONNECT", start_pad, accent);
		display_n_times(" ", panel_width - 31);
		printf("│%s\n", reset);
		printf("%s%s│", start_pad, accent);
		display_n_times(" ", panel_width - 2);
		printf("│%s\n", reset);
		if (inner_width >= (int)strlen(info)) {
			printf("%s%s│%s%s%s", start_pad, accent, soft, info, reset);
			display_n_times(" ", inner_width - strlen(info));
			printf("%s│%s\n", accent, reset);
		} else {
			printf("%s%s│%s Fill in fields and press Enter for defaults.%s", start_pad, accent, soft, reset);
			printf("%s│%s\n", accent, reset);
		}
		printf("%s%s╰", start_pad, accent);
		display_n_times("─", panel_width - 2);
		printf("╯%s\n\n", reset);
	} else {
		printf("%sQuick Splash Lobby Connect\n\n", strong);
	}

	printf("%s%sUsername%s\n", start_pad, strong, reset);
	printf("%s%s→ %s", start_pad, accent, reset);
	get_str_to_ptr(name, 32);

	printf("\n%s%sPort%s %s(default 30000)%s\n", start_pad, strong, reset, soft, reset);
	printf("%s%s→ %s", start_pad, accent, reset);
	get_str_to_ptr(port, 7);

	printf("\n%s%sServer%s %s(default 127.0.0.1)%s\n", start_pad, strong, reset, soft, reset);
	printf("%s%s→ %s", start_pad, accent, reset);
	get_str_to_ptr(addr, 30);

	if (strlen(port) == 0) strcpy(port, "30000");
	if (strlen(addr) == 0) strcpy(addr, "127.0.0.1");

	printf("\n%s%sConnecting...%s\n", start_pad, accent, reset);

	char buf[sizeof("Connecting to : as ") + 30 + 7 + 32];
	snprintf(buf, sizeof(buf), "Connecting to %s:%s as %s", addr, port, name);
	printf("%s%s%s%s\n", start_pad, soft, buf, reset);
}
