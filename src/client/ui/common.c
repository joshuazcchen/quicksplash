#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "output.h"
#include "client_ui_common.h"

void ui_print_line_plain(char* start_pad, int width, char* text) {
	if (!start_pad) start_pad = "";
	if (width < 1) width = 1;

	int format_len = snprintf(NULL, 0, "%s│%%-*.*s│\n", start_pad) + 1; // calculate format length dynamically based on start_pad length
	char format[format_len];

	snprintf(format, format_len, "%s│%%-*.*s│\n", start_pad);
	justify_text_format(format, width, width, text);
}

void ui_print_line_color(char* start_pad, int width, const char* color, char* text) {
	if (!start_pad) start_pad = "";
	if (width < 1) width = 1;
	if (!color) color = "";

	int format_len = snprintf(NULL, 0, "%s%s│ %%-*.*s │\033[0m\n", start_pad, color) + 1; // calculate format length dynamically based on start_pad and color lengths
	char format[format_len];

	snprintf(format, sizeof(format), "%s%s│ %%-*.*s │\033[0m\n", start_pad, color);
	justify_text_format(format, width, width, text);
}
