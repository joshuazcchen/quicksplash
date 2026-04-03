#ifndef CARDS_H
#define CARDS_H
#include "gamestructs.h"

Card** generate_cards();
void free_cards(Card ** cards);
Card* draw_random(Card** cards);
void free_card(Card* card);


#endif