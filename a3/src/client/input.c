#include "client_output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_text_input(int max_chars) {
	char text[max_chars + 1]; // Buffer for max_chars characters, +1 for null terminator
														//printf("Enter text (max %d chars): ", max_chars);

	if (fgets(text, sizeof(text), stdin)) { // fgets with sizeof(name) exactly gets max_chars characters and null terminates the string
		return text;
	} else {
		exit(1);
	}
}
// TODO: can we standardize the tab indentation amt to 4 spaces?
void get_str_to_ptr(char* ptr, int max_chars) {
	if (!fgets(ptr, max_chars, stdin)) {
		exit(1);
	}
    ptr[strcspn(ptr, "\n")] = '\0';
//	int c;
//	while ((c = getchar()) != '\n' && c != EOF);
//	return;
}
