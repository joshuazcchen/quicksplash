#ifndef CLIENT_UI_PROMPT_H
#define CLIENT_UI_PROMPT_H

#include "gamestructs.h"

void ui_show_card_prompt(Card card);
char* ui_collect_card_response(Card card, int max_chars);

#endif
