#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "output.h"
#include "client_ui_prompt.h"
#include "client_input.h"
#include "client_ui_common.h"

void ui_show_card_prompt(Card card) {
	int width = terminal_width;
	if (width <= 0) width = 80;

	int panel_width = (width >= 74) ? 70 : width - 2;
	if (panel_width < 34) panel_width = 34;
	if (panel_width > width) panel_width = width;

	int left_pad = (width - panel_width) / 2;
	if (left_pad < 0) left_pad = 0;

	char start_pad[left_pad + 1];
	memset(start_pad, ' ', left_pad);
	start_pad[left_pad] = '\0';

	int inner_width = panel_width - 2;

	clear_screen();
	printf("%s╭", start_pad);
	display_n_times("═", inner_width);
	printf("╮\n");
	ui_print_line_plain(start_pad, inner_width, "CARD PROMPT");
	ui_print_line_plain(start_pad, inner_width, "Write the funniest response you can.");
	printf("%s├", start_pad);
	display_n_times("─", inner_width);
	printf("┤\n");
	ui_print_line_plain(start_pad, inner_width, card.prompt_text ? card.prompt_text : "[missing prompt]");
	printf("%s╰", start_pad);
	display_n_times("═", inner_width);
	printf("╯\n\n");
}

char* ui_collect_card_response(Card card, int max_chars) {
	char hint[96];
	char* input = NULL;

	if (max_chars < 1) max_chars = 1;
	char input_buf[max_chars + 1];

	ui_show_card_prompt(card);
	snprintf(hint, sizeof(hint), "Response (%d chars max)", max_chars);

	while (1) {
		printf("\033[1;33m%s\033[0m\n", hint);
		printf("\033[0;36m> \033[0m");
		fflush(stdout);

		memset(input_buf, 0, sizeof(input_buf));
		get_str_to_ptr(input_buf, sizeof(input_buf));

		if (strcmp(input_buf, "INT") == 0) {
			return NULL;
		}

		if (strlen(input_buf) == 0) {
			printf("\033[0;31mPlease enter at least one character.\033[0m\n\n");
			continue;
		}

		input = malloc(strlen(input_buf) + 1);
		if (!input) {
			perror("malloc");
			exit(1);
		}
		strcpy(input, input_buf);

		return input;
	}
}
