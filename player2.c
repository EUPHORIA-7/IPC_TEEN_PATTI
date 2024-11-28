#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include "gamestate.h"

// Function to compare two cards
int compare_card(struct Card card1, struct Card card2) {
    return card1.value - card2.value;
}

// Function to calculate hand rank (simplified)
int calculate_hand_rank(struct Card hand[]) {
    // Sort hand by card value
    struct Card sorted_hand[3];
    memcpy(sorted_hand, hand, sizeof(struct Card) * 3);
    
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2 - i; j++) {
            if (sorted_hand[j].value > sorted_hand[j+1].value) {
                struct Card temp = sorted_hand[j];
                sorted_hand[j] = sorted_hand[j+1];
                sorted_hand[j+1] = temp;
            }
        }
    }
    
    // Check for pure sequence (consecutive values)
    if (sorted_hand[0].value + 1 == sorted_hand[1].value && 
        sorted_hand[1].value + 1 == sorted_hand[2].value) {
        return 3; // Pure Sequence
    }
    
    // Check for pair
    if (sorted_hand[0].value == sorted_hand[1].value || 
        sorted_hand[1].value == sorted_hand[2].value || 
        sorted_hand[0].value == sorted_hand[2].value) {
        return 2; // Pair
    }
    
    // High card
    return sorted_hand[2].value;
}

// Function for manual betting input
int player2_manual_bet(struct shared_struct *shared_stuff) {
    int bet;
    char choice;

    while (1) {
        printf("Your current money: $%d\n", shared_stuff->player2_money);
        printf("Current pot: $%d\n", shared_stuff->current_pot);
        printf("Current bet to match: $%d\n", shared_stuff->current_bet);
        
        // Give betting options
        printf("Betting Options:\n");
        printf("1. Call (match current bet)\n");
        printf("2. Raise\n");
        printf("3. Fold\n");
        printf("Enter your choice (1-3): ");
        
        // Get user choice
        scanf(" %c", &choice);
        
        switch(choice) {
            case '1':
                // Call current bet
                bet = shared_stuff->current_bet;
                if (bet > shared_stuff->player2_money) {
                    printf("Not enough money to call!\n");
                    continue;
                }
                return bet;
            
            case '2':
                // Raise
                printf("Enter amount to bet (must be >= current bet): ");
                scanf("%d", &bet);
                
                if (bet < shared_stuff->current_bet) {
                    printf("Bet must be at least the current bet!\n");
                    continue;
                }
                
                if (bet > shared_stuff->player2_money) {
                    printf("Not enough money!\n");
                    continue;
                }
                
                return bet;
            
            case '3':
                // Fold
                return 0;
            
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}

int main() {
    // Create shared memory
    int shmid = shmget((key_t)1234, sizeof(struct shared_struct), 0666);
    
    if (shmid == -1) {
        fprintf(stderr, "Shared memory access failed\n");
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
    
    // Wait for game to start
    while (!shared_stuff->gamestarter) {
        sleep(1);
    }
    
    // Print Player 2's hand
    char *suit_names[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
    char *value_names[] = {"Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", 
                           "Jack", "Queen", "King", "Ace"};
    
    printf("Player 2's Hand:\n");
    for (int i = 0; i < 3; i++) {
        printf("Card %d: %s of %s\n", i+1, 
               value_names[shared_stuff->player2_hand[i].value - 2], 
               suit_names[shared_stuff->player2_hand[i].suit]);
    }
    
    // Betting round with manual input
    int bet = player2_manual_bet(shared_stuff);
    if (bet == 0) {
        // Fold
        shared_stuff->player2_folded = true;
        printf("Player 2 folds.\n");
    } else {
        // Place bet
        shared_stuff->player2_money -= bet;
        shared_stuff->current_pot += bet;
        shared_stuff->current_bet = (bet > shared_stuff->current_bet) ? bet : shared_stuff->current_bet;
        printf("Player 2 bets $%d. Pot now: $%d\n", bet, shared_stuff->current_pot);
    }
    
    // Wait for game to complete
    while (!shared_stuff->game_over) {
        sleep(1);
    }
    
    // Display final result
    if (shared_stuff->winner == 2) {
        printf("Player 2 wins! Total money: $%d\n", shared_stuff->player2_money);
    } else {
        printf("Player 2 loses. Total money: $%d\n", shared_stuff->player2_money);
    }
    
    // Detach shared memory
    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "Shared memory detachment failed\n");
        fprintf(stderr, "Error: %s\n", strerror(errno)); 
        exit(EXIT_FAILURE);
    }
    
    return 0;
}