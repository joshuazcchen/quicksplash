#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h> 
#include "../../include/gamestructs.h"



Card ** generate_cards(FILE * file){
    char prompt[MAX_PROMPT_SIZE]; 
    Card **cards = malloc(sizeof(Card *) * (PROMPT_COUNT));
    // generate an array of PROMPT_COUNT pointers 
    if (!cards) {
        perror("malloc");
        exit(1);
    }
    
    int i = 0;
    while(fgets(prompt,MAX_PROMPT_SIZE,file) != NULL && i < PROMPT_COUNT){
        cards[i] = malloc(sizeof(Card)); // create a card object 
        if (!cards[i]) {
            perror("malloc");
            exit(1);
        }
        cards[i]->prompt_text = malloc(sizeof(char)*(strlen(prompt))); // already size + 1 because of newline char
        if (!cards[i]->prompt_text) {
            perror("malloc");
            exit(1);
        }
        strncpy(cards[i]->prompt_text, prompt,(strlen(prompt))); // copy over data written in prompt read from file
        (cards[i]->prompt_text)[(strlen(prompt)-1)] = '\0'; // replace newline char with null term
        i++; 
    }

    return cards; 
}

void free_cards(Card ** cards){
    for(int j = 0; j < PROMPT_COUNT;j++){
        free(cards[j]->prompt_text);
        free(cards[j]);
    }
    free(cards);
}
// draws a random card
Card* draw_random(Card** cards){
    int rand_index = rand() % (PROMPT_COUNT-1); // generate random number between 0 and PROMPT_COUNT
    printf("the random number i rolled is: %d \n ", rand_index);
    return  cards[rand_index];
}

int main(){
    FILE *prompt_file;
    srand(time(NULL));

    prompt_file = fopen("../../assets/prompts.txt","r");
    if (!prompt_file){
        perror("fopen");
    }

    Card ** cards = generate_cards(prompt_file);
    
    printf("card with prompt: %s \n",draw_random(cards)->prompt_text);

    
    // for(int i =0; i< 5;i++){
    //     printf("card with prompt: %s ",cards[i]->prompt_text);
    // }


    free_cards(cards);
    fclose(prompt_file);




    return 1; 
}

