#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <errno.h>
#include <time.h>
#include "gamestate.h"

// Function to print a card with full name
void print_card(struct Card card) {
    char *suit_names[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
    char *value_names[] = {"Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", 
                           "Jack", "Queen", "King", "Ace"};
    
    printf("%s of %s", 
           value_names[card.value - 2], 
           suit_names[card.suit]);
}

int main() {
    srand(time(NULL));
    printf("Teen Patti Game Starter PID: %d\n", (int)getpid());
    
    // Create shared memory
    int shmid = shmget((key_t)1234, sizeof(struct shared_struct), 0666 | IPC_CREAT);
    
    if (shmid == -1) {
        fprintf(stderr, "Shared memory creation failed\n");
        fprintf(stderr, "Error: %s\n", strerror(errno)); 
        exit(EXIT_FAILURE);
    }

    // Attach shared memory
    void *shared_memory = shmat(shmid, (void *)0, 0);
    
    if (shared_memory == (void *)-1) {
        fprintf(stderr, "Shared memory attachment failed\n");
        fprintf(stderr, "Error: %s\n", strerror(errno)); 
        exit(EXIT_FAILURE);
    }
    
    struct shared_struct *shared_stuff = (struct shared_struct *)shared_memory;
    
    // Deal cards to players
    printf("Dealing cards...\n");
    printf("Player 1's Hand:\n");
    for (int i = 0; i < 3; i++) {
        // Player 1 cards
        shared_stuff->player1_hand[i].value = rand() % 13 + 2;  // 2-14
        shared_stuff->player1_hand[i].suit = rand() % 4;
        
        // Print Player 1's cards
        printf("Card %d: ", i+1);
        print_card(shared_stuff->player1_hand[i]);
        printf("\n");
    }
    
    printf("\nPlayer 2's Hand:\n");
    for (int i = 0; i < 3; i++) {
        // Player 2 cards
        shared_stuff->player2_hand[i].value = rand() % 13 + 2;
        shared_stuff->player2_hand[i].suit = rand() % 4;
        
        // Print Player 2's cards
        printf("Card %d: ", i+1);
        print_card(shared_stuff->player2_hand[i]);
        printf("\n");
    }
    
    // Initialize game state
    shared_stuff->player_turn = 1;  // Player 1 starts
    shared_stuff->gamestarter = 1;  // Game is now initialized
    shared_stuff->game_over = false;
    shared_stuff->winner = 0;
    
    // Initialize betting parameters
    shared_stuff->player1_money = 1000;  // Initial money for player 1
    shared_stuff->player2_money = 1000;  // Initial money for player 2
    shared_stuff->current_pot = 0;
    shared_stuff->current_bet = 0;
    shared_stuff->player1_folded = false;
    shared_stuff->player2_folded = false;
    
    printf("\nCards dealt. Waiting for players to play...\n");
    
    // Wait until game is over
    while (!shared_stuff->game_over) {
        sleep(1);
    }
    
    // Display final game result
    if (shared_stuff->winner == 1) {
        printf("Player 1 wins the game!\n");
    } else if (shared_stuff->winner == 2) {
        printf("Player 2 wins the game!\n");
    } else {
        printf("Game ended without a winner.\n");
    }
    
    // Detach and remove shared memory
    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "Shared memory detachment failed\n");
        fprintf(stderr, "Error: %s\n", strerror(errno)); 
        exit(EXIT_FAILURE);
    }
    
    // Remove shared memory segment
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "Shared memory removal failed\n");
        fprintf(stderr, "Error: %s\n", strerror(errno)); 
        exit(EXIT_FAILURE);
    }
    
    return 0;
}