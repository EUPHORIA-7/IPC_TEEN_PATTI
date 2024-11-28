# Teen Patti Game

## Overview

This project implements a simplified version of the popular Indian card game, Teen Patti. The game involves two players competing against each other by betting and comparing their hands of cards. The game is designed to run in a multi-process environment using shared memory for communication between the game starter and the players.

## Project Structure

The project consists of the following files:

- `gamestarter.c`: This file contains the main logic for starting the game, dealing cards, and managing the game state.
- `gamestate.h`: This header file defines the data structures used in the game, including the `Card` structure and the `shared_struct` structure that holds the game state.
- `player1.c`: This file contains the logic for Player 1, including betting and hand evaluation.
- `player2.c`: This file contains the logic for Player 2, similar to Player 1.

## Dependencies

- C Standard Library
- POSIX Shared Memory

## Compilation

To compile the project, use the following command in the terminal:

```bash
gcc -o gamestarter gamestarter.c
gcc -o player1 player1.c
gcc -o player2 player2.c
