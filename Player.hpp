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
    // postcondition for initializing player starting positions

    ~Player();
    
    Player(const Player& other);
    // postcondition creating copy of other player
    
    Player& operator=(const Player& other);
    // postcondition assigns other player values to this player

    int getPlayerId() const;
    // postcondition returns the player id
    
    Node* getCurrentNode() const;
    // postcondition returns the node of current node
    
    void setCurrentNode(Node* node);
    // postcondition sets the current node to new node
    
    Node* getCastleNode() const;
    // postcondition returns castle node

    // Drawing Phase
    
    bool addCardToHand(Card* card);
    // postcondition adds a card to the hand if not full
    
    bool removeCardFromHand(Card* card);
    // postcondition removes card from the hand if present
    
    int getHandSize() const;
    // postcondition returns number of cards in hand at the time
    
    Card* getCardInHand(int index) const;
    // postcondition returns the card at the given index and nullptr if out of range
    
    bool isHandFull() const;
    // postcondition returns whether hand has reached MAX_HAND_SIZE

    // Moving Phase
    
    bool hasActivated(const Node* node) const;
    // postcondition returns whether node already has been activated this turn
    
    void markActivated(Node* landedNode);
    
    void resetActivatedCardsForNewTurn();


private:
    int playerId;
    Node* currentNode;
    Node* castleNode;                          
    Card* hand[MAX_HAND_SIZE];
    int handSize;
    std::vector<Node*> activatedCardsThisTurn;  
};

#endif 