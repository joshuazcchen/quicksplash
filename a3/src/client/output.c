#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include "gamestructs.h"
#include "client_output.h"
#include "client_input.h"
#include "output.h"

volatile int terminal_width = 0; // force the program to read the latest value instead of a cached one
volatile sig_atomic_t needs_redraw = 0;

void update_size(int sig) {
	(void)sig;
	struct winsize w;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
		terminal_width = w.ws_col;
		needs_redraw = 1;
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

static int server_select_panel_width(int width) {
	if (width >= 72) return 68;
	if (width >= 40) return width - 4;
	return width;
}

static int server_select_left_pad(int width, int panel_width) {
	int left_pad = (width - panel_width) / 2;
	if (left_pad < 0) left_pad = 0;
	return left_pad;
}

static void set_default_guest_username(char* name, int name_cap) {
	static int seeded = 0;
	int suffix;

	if (strlen(name) > 0) return;

	if (!seeded) {
		srand((unsigned int)(time(NULL) ^ getpid()));
		seeded = 1;
	}

	suffix = (rand() % 90000) + 10000;
	snprintf(name, name_cap, "Guest%d", suffix);
}

static void box_print_border(const char* start_pad, const char* color, const char* left, const char* hchar, const char* right, int inner_width) {
	const char* reset = "\033[0m";
	printf("%s%s%s", start_pad, color, left);
	display_n_times((char*)hchar, inner_width);
	printf("%s%s\n", right, reset);
}

static void box_print_line(const char* start_pad, const char* border_color, const char* text_color, int inner_width, const char* text) {
	const char* reset = "\033[0m";
	int text_len = strlen(text);
	int shown = (text_len > inner_width) ? inner_width : text_len;

	printf("%s%s║%s", start_pad, border_color, text_color);
	printf("%.*s", shown, text);
	printf("%s", reset);
	if (inner_width > shown) display_n_times(" ", inner_width - shown);
	printf("%s║%s\n", border_color, reset);
}

static void box_print_fill(const char* start_pad, const char* border_color, const char* fill_color, int inner_width, const char* fill_char) {
	const char* reset = "\033[0m";
	printf("%s%s║%s", start_pad, border_color, fill_color);
	display_n_times((char*)fill_char, inner_width);
	printf("%s%s║%s\n", reset, border_color, reset);
}

static void server_select_render(char* name, char* port, char* addr, int active_field) {
	const char* border = "\033[1;34m";
	const char* title = "\033[1;96m";
	const char* info = "\033[0;37m";
	const char* muted = "\033[2;37m";
	const char* active = "\033[1;33m";
	const char* value = "\033[1;97m";
	const char* separator = "\033[0;36m";

	int width = terminal_width;
	if (width <= 0) width = 80;

	int panel_width = server_select_panel_width(width);
	int left_pad = server_select_left_pad(width, panel_width);

	char start_pad[left_pad + 1];
	memset(start_pad, ' ', left_pad);
	start_pad[left_pad] = '\0';
	int inner_width = panel_width - 2;
	char line[256];
	const char* marker0 = (active_field == 0) ? ">" : "-";
	const char* marker1 = (active_field == 1) ? ">" : "-";
	const char* marker2 = (active_field == 2) ? ">" : "-";

	clear_screen();

	if (panel_width >= 30) {
		box_print_border(start_pad, border, "╔", "═", "╗", inner_width);
		box_print_line(start_pad, border, title, inner_width, "  QUICK SPLASH // JOIN LOBBY WITH IP");
		box_print_fill(start_pad, border, separator, inner_width, "◦");
		box_print_line(start_pad, border, info, inner_width, "  Fill in fields. Press Enter to keep defaults.");
		box_print_line(start_pad, border, muted, inner_width, "");

		snprintf(line, sizeof(line), " %s Username  : %s", marker0, (strlen(name) > 0 ? name : "[pending]"));
		box_print_line(start_pad, border, (active_field == 0 ? active : value), inner_width, line);
		snprintf(line, sizeof(line), " %s Port      : %s", marker1, (strlen(port) > 0 ? port : "[pending]"));
		box_print_line(start_pad, border, (active_field == 1 ? active : value), inner_width, line);
		snprintf(line, sizeof(line), " %s Server    : %s", marker2, (strlen(addr) > 0 ? addr : "[pending]"));
		box_print_line(start_pad, border, (active_field == 2 ? active : value), inner_width, line);

		box_print_border(start_pad, border, "╚", "═", "╝", inner_width);
		printf("\n");
	} else {
		printf("%sQuick Splash Lobby Check-In\n", title);
		printf("Username: %s\n", (strlen(name) > 0 ? name : "[pending]"));
		printf("Port: %s\n", (strlen(port) > 0 ? port : "[pending]"));
		printf("Server: %s\n\n", (strlen(addr) > 0 ? addr : "[pending]"));
	}
}

void server_select(char* name, char* port, char* addr) {
	const char* reset = "\033[0m";
	const char* accent = "\033[1;33m";
	const char* soft = "\033[2;37m";
	const char* good = "\033[1;32m";
	int width, panel_width, left_pad;
	char start_pad[128];

	needs_redraw = 0;
	server_select_render(name, port, addr, 0);
	width = terminal_width;
	if (width <= 0) width = 80;
	panel_width = server_select_panel_width(width);
	left_pad = server_select_left_pad(width, panel_width);
	if (left_pad > 127) left_pad = 127;
	memset(start_pad, ' ', left_pad);
	start_pad[left_pad] = '\0';
	printf("%s%s→ %s%s(default Guest##### | max 31 chars)%s ", start_pad, accent, reset, soft, reset);
	get_str_to_ptr(name, 32);
	set_default_guest_username(name, 32);

	if (needs_redraw) needs_redraw = 0;
	server_select_render(name, port, addr, 1);
	width = terminal_width;
	if (width <= 0) width = 80;
	panel_width = server_select_panel_width(width);
	left_pad = server_select_left_pad(width, panel_width);
	if (left_pad > 127) left_pad = 127;
	memset(start_pad, ' ', left_pad);
	start_pad[left_pad] = '\0';
	printf("%s%s→ %s%s(default 30000 | max 6 chars)%s ", start_pad, accent, reset, good, reset);
	get_str_to_ptr(port, 7);
	if (strlen(port) == 0) strcpy(port, "30000");

	if (needs_redraw) needs_redraw = 0;
	server_select_render(name, port, addr, 2);
	width = terminal_width;
	if (width <= 0) width = 80;
	panel_width = server_select_panel_width(width);
	left_pad = server_select_left_pad(width, panel_width);
	if (left_pad > 127) left_pad = 127;
	memset(start_pad, ' ', left_pad);
	start_pad[left_pad] = '\0';
	printf("%s%s→ %s%s(default 127.0.0.1 | max 29 chars)%s ", start_pad, accent, reset, good, reset);
	get_str_to_ptr(addr, 30);
	if (strlen(addr) == 0) strcpy(addr, "127.0.0.1");

	server_select_render(name, port, addr, 3);
	width = terminal_width;
	if (width <= 0) width = 80;
	panel_width = server_select_panel_width(width);
	left_pad = server_select_left_pad(width, panel_width);
	if (left_pad > 127) left_pad = 127;
	memset(start_pad, ' ', left_pad);
	start_pad[left_pad] = '\0';

	printf("\n%s%sThanks for input! You'll be joining promptly...%s\n", start_pad, accent, reset);

	char buf[sizeof("Connecting to : as ") + 30 + 7 + 32];
	snprintf(buf, sizeof(buf), "Connecting to %s:%s as %s", addr, port, name);
	printf("%s%s%s%s\n", start_pad, soft, buf, reset);
}
