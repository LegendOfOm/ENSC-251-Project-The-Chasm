//Ok so I need a bridge, two players, a game over, and a turn number
//Need a run turn, check win condition, drawing phase, placing phase, moving phase
#ifndef GAMELOGIC_HPP
#define GAMELOGIC_HPP  

#include "Player.hpp"
#include "Bridge.hpp"

class GameLogic {
public:
    GameLogic();
    // postcondition: Initializes the game logic for the start of the game

    ~GameLogic();
    // postcondition: deletes the game logic

    void startGame();
    // postcondition: starts the game loop and manages turns

private:
    Bridge bridge;
    Player player1;
    Player player2;
    int turnNumber;
    bool gameOver;

    void runTurn();
    // postcondition: runs a single turn of the game

    void checkWinCondition();
    // postcondition: checks if any player has won the game

    void drawingPhase(Player& player);
    // postcondition: handles the drawing phase for the given player

    void placingPhase(Player& player);
    // postcondition: handles the placing phase for the given player

    void movingPhase(Player& player);
    // postcondition: handles the moving phase for the given player
};
#endif