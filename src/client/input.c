#include "client_output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_text_input(char* prompt, int max_chars) {
	char* text = malloc(max_chars + 1);               // Buffer for max_chars characters, +1 for null terminator
  if (text == NULL) {
    perror("malloc");
    exit(1);
  }
  printf("%s (max %d chars): ", prompt, max_chars);   // printf("Enter text (max %d chars): ", max_chars);

	if (fgets(text, max_chars + 1, stdin)) { // fgets with sizeof(name) exactly gets max_chars characters and null terminates the string
		if (strchr(text, '\n') == NULL) {
      int c;
      while ((c = getchar()) != '\n' && c != EOF);
    } else {
      text[strcspn(text, "\n")] = '\0';
    }
    return text;
	} else {
		exit(1);
	}
}

void get_str_to_ptr(char* ptr, int max_chars) {
	if (fgets(ptr, max_chars, stdin)) {
        if (strchr(ptr, '\n') == NULL) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        } else {
            ptr[strcspn(ptr, "\n")] = '\0';
        }
    } else {
        if (ferror(stdin)) {
            perror("Error reading from stdin");
        } else if (feof(stdin)) {
            printf("End of file reached.\n");
        }
    }
}
