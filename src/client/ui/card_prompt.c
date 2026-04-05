#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "output.h"
#include "client_ui_prompt.h"

static void prompt_print_line(char* start_pad, int width, char* text) {
	char format[64];
	if (width < 1) width = 1;
	snprintf(format, sizeof(format), "%s│%%-*.*s│\n", start_pad);
	justify_text_format(format, width, width, text);
}

static char* read_line_with_limit(int max_chars) {
	char* buf = malloc(max_chars + 1);
	if (!buf) {
		perror("malloc");
		exit(1);
	}

	if (!fgets(buf, max_chars + 1, stdin)) {
		free(buf);
		return NULL;
	}

	if (strchr(buf, '\n') == NULL) {
		int c;
		while ((c = getchar()) != '\n' && c != EOF) {
		}
	} else {
		buf[strcspn(buf, "\n")] = '\0';
	}

	return buf;
}

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
	prompt_print_line(start_pad, inner_width, "CARD PROMPT");
	prompt_print_line(start_pad, inner_width, "Write the funniest response you can.");
	printf("%s├", start_pad);
	display_n_times("─", inner_width);
	printf("┤\n");
	prompt_print_line(start_pad, inner_width, card.prompt_text ? card.prompt_text : "[missing prompt]");
	printf("%s╰", start_pad);
	display_n_times("═", inner_width);
	printf("╯\n\n");
}

char* ui_collect_card_response(Card card, int max_chars) {
	char hint[96];
	char* input = NULL;

	if (max_chars < 1) max_chars = 1;

	ui_show_card_prompt(card);
	snprintf(hint, sizeof(hint), "Response (%d chars max)", max_chars);

	while (1) {
		printf("\033[1;33m%s\033[0m\n", hint);
		printf("\033[0;36m> \033[0m");
		input = read_line_with_limit(max_chars);

		if (!input) {
			printf("\nInput ended. Sending empty response.\n");
			input = malloc(1);
			if (!input) {
				perror("malloc");
				exit(1);
			}
			input[0] = '\0';
			return input;
		}

		if (strlen(input) == 0) {
			printf("\033[0;31mPlease enter at least one character.\033[0m\n\n");
			free(input);
			input = NULL;
			continue;
		}

		return input;
	}
}
