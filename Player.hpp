#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>

class Card;
struct Node;

class Player {
public:
    static const int MAX_HAND_SIZE = 5;
    static const int MAX_HAND_SIZE_CARDS_PER_TURN = 3; // cards placed in Placing Phase
    static const int CARDS_DRAWN_PER_TURN = 3;

    Player(int playerId, Node* startingCastle);
    // postcondition initializing player start positions
    ~Player();

    // Identity/Position
    int getPlayerId() const;
    // postcondition returns the player id
    Node* getCurrentNode() const;
    // postcondition returns the node of the current node
    void setCurrentNode(Node* node);
    // postcondition sets the current node to a new node
    Node* getCastleNode() const;
    // postcondition returns the castle node

    // Drawing Phase
    bool addCardToHand(Card* card);
    
    bool removeCardFromHand(Card* card); 
    
    int getHandSize() const;
    
    Card* getCardInHand(int index) const;
    
    bool isHandFull() const;
    

    // Moving Phase
    bool hasActivated(const Node* node) const;
    void markActivated(Node* landedNode);
    // marks the node as already activated
    void resetActivatedCardsForNewTurn();
    //resets all activated nodes

    // Win Condition
    bool hasWon() const;
    // whether player has won or not
    void setWon(bool won);

private:
    int playerId;
    Node* currentNode;
    Node* castleNode;                          
    Card* hand[MAX_HAND_SIZE];
    int handSize;
    bool won;
    std::vector<Node*> activatedCardsThisTurn;  
};

#endif // PLAYER_H