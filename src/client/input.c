#include <stdio.h>

char* get_text_input(int max_chars) {
    char text[max_chars + 1]; // Buffer for max_chars characters + 1 for null terminator
    printf("Enter text (max %d chars): ", max_chars);
    
    if (fgets(name, sizeof(text), stdin)) { // fgets with sizeof(name) exactly gets 20 characters and null terminates the string aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
      return text;
    else {
      exit(1);
  }
}

