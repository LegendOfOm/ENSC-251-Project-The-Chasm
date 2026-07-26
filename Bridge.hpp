#ifndef BRIDGE_H
#define BRIDGE_H

#include <string>
#include "Card.hpp"
#include "ModifierCard.hpp"
#include "NodeCard.hpp"

struct Node {
    Node* left;
    Node* right;

    NodeCard* nodeCard = nullptr;

    ModifierCard* modifierStack = nullptr;

    bool isPlayer1Castle = false;
    bool isPlayer2Castle = false;
};

class Bridge {
public:
    Bridge();
    // constructor
    // postcondition: Initializes the bridge for the start of the game
    //                creates a doubly linked list and intializes castle nodes
    
    ~Bridge();
    // destructor
    // postcondition: deletes the bridge

    // managing nodes
    bool insertCard(int leftNode, int rightNode, NodeCard* nodeCard);
    //postcondition: attaches all the temporarily attached cards onto the bridge

    bool removeNode(int targetNode);
    
    bool attachModifierCard(int targetNode, const ModifierCard* modifier);
    
    // validation
    bool isValidNodePlacement(int targetNode, int rightNode) const;
    // postcondition: checks if you can place a card inbetween the two nodes 
    // returns true if you can and false if you cannot
    
    bool isValidModifierPlacement(int target, const ModifierCard* modifier) const; 
    // postcondition: checks if you can place the card on the node 
    // returns true if you can and false if you cannot

    int castleNodeCheck(Node* node);

    // ---- Text UI support ----
    void printBridge() const;
    // postcondition: prints the bridge

private:
    Node* player1Castle;
    Node* player2Castle;

    int numberOfNodes;

    bool areAdjacent(const Node* a, const Node* b) const;
    void relinkAround(Node* target);  
};

#endif // BRIDGE_H