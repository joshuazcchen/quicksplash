#include "gamestructs.h"
#include "gamestates.h"
#include "cards.h"
#include "comms.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include "server_comms.h"
#include "protocol.h"
extern Player players[LOBBY_SIZE];
extern int PLR_COUNT;
Card **cards;
Card * drawn_card;

// all logic of intializing the game should go here
response setup_game() {
    for(int i = 0; i < PLR_COUNT; i++){
         players[i].round_wins = 0;
    }
    cards = generate_cards();
    return GAME_SUCCESS;
}

// start the round of the game
response play_round() {
    for(int i = 0; i < PLR_COUNT; i++){
         players[i].round_votes = 0;
    }
    drawn_card = draw_random(cards);
    if(drawn_card != NULL){
        return GAME_SUCCESS;
    }
    return GAME_FAIL;
}

response await_responses() {
    printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m There are currently %d players in the Lobby.\n",PLR_COUNT);
    drawn_card->responses = calloc(LOBBY_SIZE, sizeof(Response*));
    for(int i = 0; i < PLR_COUNT; i++){
		if (players[i].fd > 2) {
			drawn_card->responses[i] = malloc(sizeof(Response)); //allocate space to be filled later
			drawn_card->responses[i]->player = &players[i]; // iterate through player array 
			drawn_card->responses[i]->response = NULL; 
		} else {
			drawn_card->responses[i] = NULL;
		}
    }
    
	Packet p = ctop(*(drawn_card)); 
    p.header.type = PKT_CARD;
    if(s_send(&p) == GAME_SUCCESS){
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Sent players prompt.\n");
    }
	free(p.data);

    if(s_listen(60) == TIMEOUT){ // time limit to check for responses
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Recorded all player responses or timed out.\n");
        for(int i = 0; i < PLR_COUNT; i++){
            for(int j = 0; j < PLR_COUNT; j++){
                if(drawn_card->responses[j]->player->p_id == players[i].p_id){

					if (players[i].fd == -1) {
						drawn_card->responses[j]->response = NULL;
                        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Player PID %d named %s has disconnected or never replied.\n", players[i].p_id, players[i].name);
					} else if (players[i].ready) {
						char* p = pkttostr(&(players[i].active));
						if (p != NULL) {
							drawn_card->responses[j]->response = strdup(pkttostr(&(players[i].active)));
							printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Stored response %s for player PID %d in Response array.\n", drawn_card->responses[j]->response,drawn_card->responses[j]->player->p_id);
						} else {

							printf("\033[1;35m[ SERVER ]\033[0m\033[1;31m gamestates.c:\033[0m No response received for player PID %d in Response array.\n", drawn_card->responses[j]->player->p_id);
						}
						players[i].ready = 0;
                        free(players[i].active.data);
                        players[i].active.data = NULL;
					}
                }
                
            }
        }
    }


    // server goes here to receive player resposnses
    return GAME_SUCCESS;
}

//end the round
response end_round() {
    free_card(drawn_card);
    drawn_card = NULL;

	for (int i = 0; i < LOBBY_SIZE; i++) {
		if (players[i].state == READY && players[i].fd == -1) {
            printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Player PID %d with Name %s disconnected.\n",players[i].p_id,players[i].name);

			char leave_msg[40];
			sprintf(leave_msg, "%d %s", players[i].p_id, players[i].name);
			Packet leave_pkt = strtopkt(PKT_PLR_DC, leave_msg);
			s_send(&leave_pkt);
			free(leave_pkt.data);

			players[i].state = DISCONNECTED;
			PLR_COUNT--;
		}
	}
    return GAME_SUCCESS;
}

response wrap_up_game() {
    Packet p = strtopkt(PKT_GAME_END, "GMAE OVER");
    if(s_send(&p) == GAME_SUCCESS){
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Sent packet to terminate clients.\n");
    }
    free(p.data);

     for(int i = 0; i < PLR_COUNT; i++){
         players[i].round_wins = 0;
    }
    free_cards(cards);
    cards = NULL;
    return GAME_SUCCESS;
}

response player_join() {
    return GAME_SUCCESS;
}

response player_leave() {
    return GAME_SUCCESS;
}

response initiate_vote() {

	Packet p = ctop(*(drawn_card));
    p.header.type = PKT_VOTE;
    if(s_send(&p) == GAME_SUCCESS){
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Sent players responses to vote on.\n");
    }
	free(p.data);

    if(s_listen(60) == TIMEOUT){ // time limit to check for responses
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Received all votes or timed out, server is ready to tally votes.\n");
        for(int i = 0; i < PLR_COUNT; i++){
			if (players[i].fd == -1 || players[i].ready == 0) {
                printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m No vote from %d\n", players[i].p_id);
				continue;
			}

            char *vote_str = pkttostr(&players[i].active);
            char *endptr = NULL;
            long voted_pid = strtol(vote_str, &endptr, 10);
            if (vote_str == NULL || endptr == vote_str || *endptr != '\0') {
                printf("\033[1;35m[ SERVER ]\033[0m\033[1;31m gamestates.c:\033[0m Invalid vote payload from player PID %d.\n", players[i].p_id);
                if (players[i].active.data != NULL) {
                    free(players[i].active.data);
                    players[i].active.data = NULL;
                }
                players[i].ready = 0;
                continue;
            }

            printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Checking what player PID %d voted; they voted %ld.\n", players[i].p_id, voted_pid);
            for(int j = 0; j < PLR_COUNT; j++){
                // ASSUME THAT WHAT THE PLAYER SENDS BACK IS THE PID OF WHO IT VOTED 
                if (drawn_card->responses[j] == NULL || drawn_card->responses[j]->player == NULL) {
                    continue;
                }
                if(drawn_card->responses[j]->player->p_id == voted_pid){
                    drawn_card->responses[j]->player->round_votes++; 
                    printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Player PID %d now has %d votes.\n",drawn_card->responses[j]->player->p_id,drawn_card->responses[j]->player->round_votes);
                    break;
                }
            }

            if (players[i].active.data != NULL) {
                free(players[i].active.data);
                players[i].active.data = NULL;
            }
            players[i].ready = 0;
        }
    }
    
    return GAME_SUCCESS;
}

response determine_round_winner(){
    int max_index = 0; 
    for (int i = 1; i < PLR_COUNT; i++) {
        if (players[i].round_votes > players[max_index].round_votes) {
            max_index = i;
        }
    }
    int tie_count = 0; 
    for(int i = 0; i < PLR_COUNT; i++){
        if(players[max_index].round_votes == players[i].round_votes){
            tie_count++;
        }
    }
    if(tie_count > 1){
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m A Tie has between two or more players.\n");
        for(int i = 0; i < PLR_COUNT;i++){
            if (players[i].round_votes == players[max_index].round_votes) {
                players[i].round_wins += 1;
            }
        }
    }
    else{
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Player PID %s wins the round with %d votes.\n", players[max_index].name,players[max_index].round_votes);
        players[max_index].round_wins += 1;
    }

    return GAME_SUCCESS;
}

response send_round_results() {
    Card results_card;
    memset(&results_card, 0, sizeof(Card));
    results_card.prompt_text = drawn_card->prompt_text;
    results_card.responses = calloc(LOBBY_SIZE, sizeof(Response*));
    if (results_card.responses == NULL) {
        perror("calloc");
        return GAME_FAIL;
    }

    for (int i = 0; i < PLR_COUNT; i++) {
        if (drawn_card->responses == NULL || drawn_card->responses[i] == NULL || drawn_card->responses[i]->player == NULL) {
            continue;
        }

        results_card.responses[i] = malloc(sizeof(Response));
        if (results_card.responses[i] == NULL) {
            perror("malloc");
            for (int j = 0; j < i; j++) {
                if (results_card.responses[j] != NULL) {
                    free(results_card.responses[j]->response);
                    free(results_card.responses[j]);
                }
            }
            free(results_card.responses);
            return GAME_FAIL;
        }

        results_card.responses[i]->player = drawn_card->responses[i]->player;
        int response_len = snprintf(NULL, 0, "%s - %d vote%s", drawn_card->responses[i]->response ? drawn_card->responses[i]->response : "[no response]", drawn_card->responses[i]->player->round_votes, drawn_card->responses[i]->player->round_votes == 1 ? "" : "s");
        results_card.responses[i]->response = malloc(response_len + 1);
        if (results_card.responses[i]->response == NULL) {
            perror("malloc");
            free(results_card.responses[i]);
            results_card.responses[i] = NULL;
            for (int j = 0; j < i; j++) {
                if (results_card.responses[j] != NULL) {
                    free(results_card.responses[j]->response);
                    free(results_card.responses[j]);
                }
            }
            free(results_card.responses);
            return GAME_FAIL;
        }

        snprintf(results_card.responses[i]->response, response_len + 1, "%s - %d vote%s", drawn_card->responses[i]->response ? drawn_card->responses[i]->response : "[no response]", drawn_card->responses[i]->player->round_votes, drawn_card->responses[i]->player->round_votes == 1 ? "" : "s");
    }

    Packet p = ctop(results_card);
    if (p.data == NULL) {
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;31m gamestates.c:\033[0m Failed to convert round results to packet.\n");
        for (int i = 0; i < PLR_COUNT; i++) {
            if (results_card.responses[i] != NULL) {
                free(results_card.responses[i]->response);
                free(results_card.responses[i]);
            }
        }
        free(results_card.responses);
        return GAME_FAIL;
    }
    p.header.type = PKT_STATS;
    if(s_send(&p) == GAME_SUCCESS){
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Sent round results to clients.\n");
    } else {
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;31m gamestates.c:\033[0m Failed to send round results to clients.\n");
        free(p.data);
        for (int i = 0; i < PLR_COUNT; i++) {
            if (results_card.responses[i] != NULL) {
                free(results_card.responses[i]->response);
                free(results_card.responses[i]);
            }
        }
        free(results_card.responses);
        return GAME_FAIL;
    }
    free(p.data);

    for (int i = 0; i < PLR_COUNT; i++) {
        if (results_card.responses[i] != NULL) {
            free(results_card.responses[i]->response);
            free(results_card.responses[i]);
        }
    }
    free(results_card.responses);
    return GAME_SUCCESS;
}

response wait_for_host_next_round() {
    int host_index = -1;
    for (int i = 0; i < PLR_COUNT; i++) { // find host
        if (players[i].p_id == 1 && players[i].fd > 2 && players[i].state != DISCONNECTED) {
            host_index = i;
            break;
        }
    }

    if (host_index == -1) { // not found host
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;31m gamestates.c:\033[0m No host is available to start the next round.\n");
        return GAME_FAIL;
    }

    printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Waiting for host PID %d to start the next round.\n", players[host_index].p_id);

    while (1) {
        if (players[host_index].fd == -1 || players[host_index].state == DISCONNECTED) { // host disconnect
            printf("\033[1;35m[ SERVER ]\033[0m\033[1;31m gamestates.c:\033[0m Host disconnected before starting the next round.\n");
            return GAME_FAIL;
        }

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(players[host_index].fd, &fds);

        struct timeval tl;
        tl.tv_sec = 1;
        tl.tv_usec = 0;

        int ready = select(players[host_index].fd + 1, &fds, NULL, NULL, &tl);
        if (ready <= 0) {
            continue;
        }

        if (FD_ISSET(players[host_index].fd, &fds)) {
            response ret = s_read(&players[host_index]);
            if (ret == READ_SUCCESS && players[host_index].ready) {
                if (players[host_index].active.header.type == PKT_START) {
                    printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Host started the next round.\n");
                    if (players[host_index].active.data != NULL) {
                        free(players[host_index].active.data);
                        players[host_index].active.data = NULL;
                    }
                    players[host_index].ready = 0;
                    return GAME_SUCCESS;
                }

                if (players[host_index].active.data != NULL) {
                    free(players[host_index].active.data);
                    players[host_index].active.data = NULL;
                }
                players[host_index].ready = 0;
            } else if (ret == CLIENT_DISCONNECT) {
                printf("\033[1;35m[ SERVER ]\033[0m\033[1;31m gamestates.c:\033[0m Host disconnected while waiting for the next round.\n");
                return GAME_FAIL;
            }
        }
    }
}

response determine_game_winner(){
    int max_index = 0; 
    for (int i = 1; i < PLR_COUNT; i++) {
        if (players[i].round_wins > players[max_index].round_wins) {
            max_index = i;
        }
    }
    int tie_count = 0; 
    for(int i = 0; i < PLR_COUNT; i++){
        if(players[max_index].round_wins == players[i].round_wins){
            tie_count++;
        }
    }
    if(tie_count > 1){
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m A Tie has between two or more players.\n");
        for(int i = 0; i < PLR_COUNT;i++){
            if (players[i].round_wins == players[max_index].round_wins) {
                printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m %s wins the game with %d points.\n",players[i].name ,players[i].round_wins);
            }
        }
    }
    else{
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m %s wins the game with %d points.\n",players[max_index].name ,players[max_index].round_wins);
    }
    
   
    return GAME_SUCCESS;
}

response send_game_results() {
    Card results_card;
    memset(&results_card, 0, sizeof(Card));

    results_card.prompt_text = malloc(MAX_PROMPT_SIZE);
    if (results_card.prompt_text == NULL) {
        perror("malloc");
        return GAME_FAIL;
    }
    snprintf(results_card.prompt_text, MAX_PROMPT_SIZE, "FINAL GAME RESULTS");

    results_card.responses = calloc(LOBBY_SIZE, sizeof(Response*));
    if (results_card.responses == NULL) {
        perror("calloc");
        free(results_card.prompt_text);
        results_card.prompt_text = NULL;
        return GAME_FAIL;
    }

    for (int i = 0; i < PLR_COUNT; i++) {
        if (players[i].fd <= 2 || players[i].state == DISCONNECTED) {
            continue;
        }

        results_card.responses[i] = malloc(sizeof(Response));
        if (results_card.responses[i] == NULL) {
            perror("malloc");
            for (int j = 0; j < i; j++) {
                if (results_card.responses[j] != NULL) {
                    free(results_card.responses[j]->response);
                    free(results_card.responses[j]);
                }
            }
            free(results_card.responses);
            free(results_card.prompt_text);
            return GAME_FAIL;
        }

        results_card.responses[i]->player = &players[i];
        results_card.responses[i]->response = malloc(32);
        if (results_card.responses[i]->response == NULL) {
            perror("malloc");
            free(results_card.responses[i]);
            results_card.responses[i] = NULL;
            for (int j = 0; j < i; j++) {
                if (results_card.responses[j] != NULL) {
                    free(results_card.responses[j]->response);
                    free(results_card.responses[j]);
                }
            }
            free(results_card.responses);
            free(results_card.prompt_text);
            return GAME_FAIL;
        }

        snprintf(results_card.responses[i]->response, 32, "%d point%s", players[i].round_wins, players[i].round_wins == 1 ? "" : "s");
    }

    Packet p = ctop(results_card);
    if (p.data == NULL) {
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;31m gamestates.c:\033[0m Failed to convert game results to packet.\n");
        for (int i = 0; i < PLR_COUNT; i++) {
            if (results_card.responses[i] != NULL) {
                free(results_card.responses[i]->response);
                free(results_card.responses[i]);
            }
        }
        free(results_card.responses);
        free(results_card.prompt_text);
        return GAME_FAIL;
    }

    p.header.type = PKT_STATS;
    if (s_send(&p) == GAME_SUCCESS) {
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;94m gamestates.c:\033[0m Sent final game results to clients.\n");
    } else {
        printf("\033[1;35m[ SERVER ]\033[0m\033[1;31m gamestates.c:\033[0m Failed to send final game results to clients.\n");
        free(p.data);
        for (int i = 0; i < PLR_COUNT; i++) {
            if (results_card.responses[i] != NULL) {
                free(results_card.responses[i]->response);
                free(results_card.responses[i]);
            }
        }
        free(results_card.responses);
        free(results_card.prompt_text);
        return GAME_FAIL;
    }

    free(p.data);

    for (int i = 0; i < PLR_COUNT; i++) {
        if (results_card.responses[i] != NULL) {
            free(results_card.responses[i]->response);
            free(results_card.responses[i]);
        }
    }
    free(results_card.responses);
    free(results_card.prompt_text);

    return GAME_SUCCESS;
}
