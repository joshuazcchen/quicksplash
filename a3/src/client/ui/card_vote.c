#include <stdio.h>
#include <string.h>
#include "output.h"
#include "client_ui_vote.h"

static void vote_print_line(char* pad, int width, char* text) {
	char format[64];
	if (width < 1) width = 1;
	snprintf(format, sizeof(format), "%s│%%-%d.%ds│\n", pad, width, width);
	justify_text_format(format, width, width, text);
}

void ui_show_vote_card(Card card, int response_count) {
	int total_width = terminal_width;
	if (total_width <= 0) total_width = 80;

	int panel_width = (total_width >= 70) ? 68 : total_width - 2;
	if (panel_width < 32) panel_width = 32;
	if (panel_width > total_width) panel_width = total_width;

	int left_pad = (total_width - panel_width) / 2;
	if (left_pad < 0) left_pad = 0;

	char start_pad[left_pad + 1];
	memset(start_pad, ' ', left_pad);
	start_pad[left_pad] = '\0';

	int inner_width = panel_width - 2;
	char line[1024];

	clear_screen();

	printf("%s╭", start_pad);
	display_n_times("─", inner_width);
	printf("╮\n");

	vote_print_line(start_pad, inner_width, "VOTE NOW");
	vote_print_line(start_pad, inner_width, "Original card:");
	vote_print_line(start_pad, inner_width, card.prompt_text ? card.prompt_text : "[missing prompt]");

	printf("%s├", start_pad);
	display_n_times("─", inner_width);
	printf("┤\n");

	vote_print_line(start_pad, inner_width, "Responses:");

	if (!card.responses || response_count <= 0) {
		vote_print_line(start_pad, inner_width, "No responses were included in this packet yet.");
	} else {
		for (int i = 0; i < response_count; i++) {
			Response* response = card.responses[i];
			char* response_text = "[empty response]";
			char* submitter = "Unknown";

			if (response) {
				if (response->response && strlen(response->response) > 0) response_text = response->response;
				if (response->player && strlen(response->player->name) > 0) submitter = response->player->name;
			}

			snprintf(line, sizeof(line), "%d) %s", i + 1, response_text);
			vote_print_line(start_pad, inner_width, line);
			snprintf(line, sizeof(line), "   submitted by: %s", submitter);
			vote_print_line(start_pad, inner_width, line);
		}
	}

	printf("%s╰", start_pad);
	display_n_times("─", inner_width);
	printf("╯\n");
}