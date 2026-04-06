#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "output.h"
#include "client_ui_vote.h"
#include "client_ui_common.h"



void ui_show_vote_card(Card card, int response_count) {
	int total_width = terminal_width;
	if (total_width <= 0) total_width = 80;

	int panel_width = (total_width >= 90) ? 86 : total_width - 2;
	if (panel_width < 44) panel_width = 44;
	if (panel_width > total_width) panel_width = total_width;

	int left_pad = (total_width - panel_width) / 2;
	if (left_pad < 0) left_pad = 0;

	char start_pad[left_pad + 1];
	memset(start_pad, ' ', left_pad);
	start_pad[left_pad] = '\0';

	int inner_width = panel_width - 2;
	int content_width = inner_width - 2;
	char line[1024] = {0};
	int shown_count = 0;

	clear_screen();

	printf("%s\033[38;5;45m╭", start_pad);
	display_n_times("─", inner_width);
	printf("╮\033[0m\n");

	ui_print_line_color(start_pad, content_width, "\033[1;38;5;45m", "VOTE RESULTS BOARD");
	ui_print_line_color(start_pad, content_width, "\033[38;5;252m", "Prompt:");
	ui_print_line_color(start_pad, content_width, "\033[1;38;5;230m", card.prompt_text ? card.prompt_text : "[missing prompt]");

	printf("%s\033[38;5;45m├", start_pad);
	display_n_times("─", inner_width);
	printf("┤\033[0m\n");

	ui_print_line_color(start_pad, content_width, "\033[1;38;5;39m", "Responses:");
	ui_print_line_color(start_pad, content_width, "\033[38;5;244m", "Type the response number and press Enter to cast your vote.");

	if (!card.responses || response_count <= 0) {
		ui_print_line_color(start_pad, content_width, "\033[38;5;203m", "No responses were included in this packet yet.");
	} else {
		for (int i = 0; i < response_count; i++) {
			Response* response = card.responses[i];
			char* response_text = "[empty response]";
			char* submitter = "Unknown";

			if (!response) {
				continue;
			}

			if (response->response && strlen(response->response) > 0) response_text = response->response;
			if (response->player && strlen(response->player->name) > 0) submitter = response->player->name;

			if (shown_count > 0) {
				ui_print_line_color(start_pad, content_width, "\033[38;5;240m", "----------------------------------------");
			}

			snprintf(line, sizeof(line), "[%d] %s", i + 1, submitter);
			ui_print_line_color(start_pad, content_width, "\033[1;38;5;51m", line);

			ui_print_line_color(start_pad, content_width, "\033[38;5;255m", response_text);
			shown_count++;
		}
	}

	if (shown_count == 0) {
		ui_print_line_color(start_pad, content_width, "\033[38;5;203m", "No active responses available to vote on.");
	}

	printf("%s\033[38;5;45m╰", start_pad);
	display_n_times("─", inner_width);
	printf("╯\033[0m\n\n");

	if (response_count > 0) {
		printf("%s\033[1;38;5;118mEnter choice [1-%d]: \033[0m", start_pad, response_count);
	} else {
		printf("%s\033[1;38;5;203mEnter choice: \033[0m", start_pad);
		}
}

void ui_show_vote_results(char* results_text) {
	int total_width = terminal_width;
	if (total_width <= 0) total_width = 80;

	int panel_width = (total_width >= 90) ? 86 : total_width - 2;
	if (panel_width < 44) panel_width = 44;
	if (panel_width > total_width) panel_width = total_width;

	int left_pad = (total_width - panel_width) / 2;
	if (left_pad < 0) left_pad = 0;

	char start_pad[left_pad + 1];
	memset(start_pad, ' ', left_pad);
	start_pad[left_pad] = '\0';

	int inner_width = panel_width - 2;
	int content_width = inner_width - 2;

	clear_screen();

	printf("%s\033[38;5;214m╭", start_pad);
	display_n_times("─", inner_width);
	printf("╮\033[0m\n");

	ui_print_line_color(start_pad, content_width, "\033[1;38;5;214m", "ROUND RESULTS");
	printf("%s\033[38;5;214m├", start_pad);
	display_n_times("─", inner_width);
	printf("┤\033[0m\n");

	char* copy = strdup(results_text ? results_text : "No results received.");
	if (copy == NULL) {
		ui_print_line_color(start_pad, content_width, "\033[38;5;203m", "Could not display vote results.");
	} else {
		char* saveptr = NULL;
		char* line = strtok_r(copy, "\n", &saveptr);
		while (line != NULL) {
			if (strlen(line) == 0) {
				ui_print_line_color(start_pad, content_width, "\033[38;5;244m", " ");
			} else {
				ui_print_line_color(start_pad, content_width, "\033[38;5;255m", line);
			}
			line = strtok_r(NULL, "\n", &saveptr);
		}
		free(copy);
	}

	printf("%s\033[38;5;214m╰", start_pad);
	display_n_times("─", inner_width);
	printf("╯\033[0m\n");
}
