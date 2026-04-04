#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "output.h"
#include "client_input.h"
#include "client_ui_lobby.h"

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

static void box_print_border(char* start_pad, char* color, char* left, char* hchar, char* right, int inner_width) {
	char* reset = "\033[0m";
	printf("%s%s%s", start_pad, color, left);
	display_n_times((char*)hchar, inner_width);
	printf("%s%s\n", right, reset);
}

static void box_print_line(char* start_pad, char* border_color, char* text_color, int inner_width, char* text) {
	char* reset = "\033[0m";
	int text_len = strlen(text);
	int shown = (text_len > inner_width) ? inner_width : text_len;

	printf("%s%s║%s", start_pad, border_color, text_color);
	printf("%.*s", shown, text);
	printf("%s", reset);
	if (inner_width > shown) display_n_times(" ", inner_width - shown);
	printf("%s║%s\n", border_color, reset);
}

static void box_print_fill(char* start_pad, char* border_color, char* fill_color, int inner_width, char* fill_char) {
	char* reset = "\033[0m";
	printf("%s%s║%s", start_pad, border_color, fill_color);
	display_n_times((char*)fill_char, inner_width);
	printf("%s%s║%s\n", reset, border_color, reset);
}

static void server_select_render(char* name, char* port, char* addr, int active_field) {
	char* border = "\033[1;34m";
	char* title = "\033[1;96m";
	char* info = "\033[0;37m";
	char* muted = "\033[2;37m";
	char* active = "\033[1;33m";
	char* value = "\033[1;97m";
	char* separator = "\033[0;36m";

	int width = terminal_width;
	if (width <= 0) width = 80;

	int panel_width = server_select_panel_width(width);
	int left_pad = server_select_left_pad(width, panel_width);

	char start_pad[left_pad + 1];
	memset(start_pad, ' ', left_pad);
	start_pad[left_pad] = '\0';
	int inner_width = panel_width - 2;
	char line[256];
	char* marker0 = (active_field == 0) ? ">" : "-";
	char* marker1 = (active_field == 1) ? ">" : "-";
	char* marker2 = (active_field == 2) ? ">" : "-";

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

void ui_server_select(char* name, char* port, char* addr) {
	char* reset = "\033[0m";
	char* accent = "\033[1;33m";
	char* soft = "\033[2;37m";
	char* good = "\033[1;32m";
	int width, panel_width, left_pad;
	char start_pad[128];

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
