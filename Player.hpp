#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Card;
struct Node;

class Player {
public:
    static const int MAX_HAND_SIZE = 5;
    static const int MAX_HAND_SIZE_CARDS_PER_TURN = 3; // cards placed per Placing Phase
    static const int CARDS_DRAWN_PER_TURN = 3;

    Player(int playerId, Node* startingCastle);
    // postcondition: initializes the player starting positions

    ~Player();

    // ---- Identity / position ----
    int getPlayerId() const;
    // postcondition: returns the player id

    Node* getCurrentNode() const;
    // postcondition: returns the node of the current node;

    void setCurrentNode(Node* node);
    // postcondition: sets the current node to a new node

    Node* getCastleNode() const;
    // postcondition: returns the castle node

    // ---- Hand management (Drawing Phase) ----
    bool addCardToHand(Card* card);      
    // precondition: card must be valid and hand must not be full
    // postcondition: adds a card to the hand and returns true

    bool removeCardFromHand(Card* card); // used when a card is placed from hand
    // postcondition: removes a card from the players hand

    int getHandSize() const;
    // postcondition: returns the amount of cards in the player's hand

    Card* getCardInHand(int index) const;
    // precondition: index must be within the amount of cards the player has
    // postcondition: returns the card selected

    bool isHandFull() const;
    // postcondition: checks whether or not the hand is full

    // ---- Movement (Moving Phase) ----
    bool hasActivated(const Node* node) const;
    //returns whether or not 

    void markActivated(Node* landedNode);
    //postcondition: marks the node as already activated

    void resetActivatedCardsForNewTurn();
    //postcondition: resets all activated nodes 

    // ---- Win condition ----
    bool hasWon() const;
    // postcondition: checks whether the player has won. 

    void setWon(bool won);
    // postcondition: sets the player status as won 

private:
    int playerId;              
    Node* currentNode;   

    Card* hand[MAX_HAND_SIZE];               
    int handSize;

    bool won;
};

#endif // PLAYER_H