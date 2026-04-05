#include "client_output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_line_into(char* buf, int cap) {
  if (buf == NULL || cap <= 0) {
    return -1;
  }

  if (!fgets(buf, cap, stdin)) {
    return 0;
  }

  if (strchr(buf, '\n') == NULL) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
  } else {
    buf[strcspn(buf, "\n")] = '\0';
  }

  return 1;
}

char* get_text_input(char* prompt, int max_chars) {
  if (max_chars < 1) max_chars = 1;

  char* text = malloc(max_chars + 1); // max_chars + null terminator
  if (text == NULL) {
    perror("malloc");
    exit(1);
  }

  printf("%s (max %d chars): ", prompt, max_chars);

  if (read_line_into(text, max_chars + 1) <= 0) {
    exit(1);
  }

  return text;
}

void get_str_to_ptr(char* ptr, int max_chars) {
    if (max_chars < 1) {
        return;
    }

    int result = read_line_into(ptr, max_chars);
    if (result == 1) {
        return;
    } else if (result == -1) {
        fprintf(stderr, "Invalid arguments to get_str_to_ptr\n");
        exit(1);
    } else { // result == 0, EOF or error
        if (result == 0) {
            if (ferror(stdin)) {
                perror("Error reading from stdin");
            } else if (feof(stdin)) {
                printf("End of file reached.\n");
            }
        }
    }
}
