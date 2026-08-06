#ifndef GAMELOGIC_HPP
#define GAMELOGIC_HPP

#include "Player.hpp"
#include "Bridge.hpp"
#include "Deck.hpp"

class Card;

class GameLogic
{
public:
    GameLogic();
    ~GameLogic();

    void startGame();

private:
    struct placementChoice
    {
        Player* player;
        Card* card;

        int cardIndex;
        int leftNode;
        int rightNode;
        int targetNode;

        bool valid;
    };

    Bridge bridge;
    Player player1;
    Player player2;
    int turnNumber;
    bool gameOver;
    bool applyPlacement(placementChoice& choice);
    Deck deck;

    void runTurn();

    void resolvePlacements(placementChoice& p1choice, placementChoice& p2choice);

    void checkWinCondition();

    void drawingPhase(Player& player);

    placementChoice placingPhase(Player& player);

    void movingPhase(Player& player);
};

#endif