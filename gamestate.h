#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <stdbool.h>

// Card structure
struct Card {
    int value;  // 2-14 (2-10, 11=Jack, 12=Queen, 13=King, 14=Ace)
    int suit;   // 0=Hearts, 1=Diamonds, 2=Clubs, 3=Spades
};

// Shared memory structure
struct shared_struct {
    // Game state variables
    int player_turn;
    bool gamestarter;
    bool game_over;
    int winner;

    // Player hands
    struct Card player1_hand[3];
    struct Card player2_hand[3];

    // Betting parameters
    int player1_money;
    int player2_money;
    int current_pot;
    int current_bet;
    bool player1_folded;
    bool player2_folded;
};

// Function prototypes
int compare_hands(struct Card hand1[], struct Card hand2[]);
int calculate_hand_rank(struct Card hand[]);

#endif // GAMESTATE_H