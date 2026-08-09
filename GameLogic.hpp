#ifndef GAMELOGIC_HPP
#define GAMELOGIC_HPP

#include "Player.hpp"
#include "Bridge.hpp"
#include "Deck.hpp"

class Card;
class PortalNode;

class GameLogic
{
public:
    // postcondition: initializes the game logic, bridge, players, turn number, and game state
    GameLogic();

    // postcondition: destroys the game logic object
    ~GameLogic();

    // postcondition: starts the game and continues running turns until the game ends
    void startGame();

private:
    // stores all the information needed for a player's placement choice
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

    // precondition: choice must contain a valid player, card, and placement location
    // postcondition: applies the selected card placement to the bridge
    // removes the card from the player's hand if the placement succeeds
    // returns true if the placement succeeds and false if it does not
    bool applyPlacement(placementChoice& choice);

    Deck deck;

    // postcondition: runs one full turn of the game
    // includes the drawing, placing, and moving phases
    void runTurn();

    // precondition: p1choice and p2choice contain the choices made by both players
    // postcondition: resolves both placement choices in the correct order
    void resolvePlacements(placementChoice& p1choice, placementChoice& p2choice);

    // postcondition: checks whether either player has reached the opposing castle
    // updates gameOver if a player wins or both players reach the castles
    void checkWinCondition();

    // precondition: player must be a valid player in the game
    // postcondition: draws cards for the player during their drawing phase
    // stops drawing if the player's hand is full
    void drawingPhase(Player& player);

    // precondition: player must be a valid player in the game
    // postcondition: gets the card and placement choice from the player
    // returns the completed placement choice
    // returns an invalid placement choice if the player skips
    placementChoice placingPhase(Player& player);

    // precondition: player must be a valid player in the game
    // postcondition: moves the player and activates movement effects until movement stops
    void movingPhase(Player& player);

    // precondition: player must be a valid player in the game
    // postcondition: moves the player by the given amount
    // positive amounts move forward and negative amounts move backward
    void movePlayer(Player& player, int amount);

    // precondition: targetNode must be a node on the bridge
    // postcondition: returns the index of the target node
    // returns -1 if the node cannot be found
    int getNodeIndex(const Node* targetNode) const;

    // precondition: node must be a valid node
    // postcondition: calculates and returns the movement amount of the node
    // includes the node card and attached modifier effects
    int calculateMovement(Node* node);

    // precondition: newPortal must be a valid portal node
    // postcondition: links the portal to an available matching portal if one exists
    void linkPortal(PortalNode* newPortal);
};

#endif