#include <stdio.h>

int main() {
    char name[21]; // Buffer for 20 characters + 1 for null terminator
    printf("Enter name (max 20 chars): ");
    
    if (fgets(name, sizeof(name), stdin)) { // fgets with sizeof(name) exactly gets 20 characters and null terminates the string aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
        printf("You entered: %s", name);
    }
    return 0;
}

