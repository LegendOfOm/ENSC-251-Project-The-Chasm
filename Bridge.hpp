#ifndef BRIDGE_H
#define BRIDGE_H

#include <string>
#include "Card.hpp"
#include "ModifierCard.hpp"

struct Node {
    Node* right;                      
    Node* left;

    NodeCard* nodeCard;

    ModifierCard* modifierStack;

    bool isPlayer1Castle;
    bool isPlayer2Castle;
};

class Bridge {
public:
    Bridge();
    // postcondition: Initializes the bridge for the start of the game
    //                creates a doubly linked list and intializes castle nodes
    
    ~Bridge();
    // postcondition: delets the bridge

    // ---- Placing Phase ----
    bool temporaryilyinsertNodeCard(int leftNode, int rightNode, NodeCard* card);
    // precondition: leftnode and rightnode indicate the place the player wants
    // place a card. leftnode and right node must be next to each other. 
    // postcondition: returns true if the card got placed. returns false if 
    // the placement conditions are not met

    bool temporaryilyAttachModifierCard(int targetNode, ModifierCard* modifier);
    // precondition: determine whether or not the modifier is valid
    // postcondition: returns true if the modefier card was added successfully
    // returns false if the modefier card was add unsuccessfully

    bool isValidNodePlacement(int targetNode, int rightNode) const;
    // postcondition: checks if you can place a card inbetween the two nodes 
    // returns true if you can and false if you cannot

    bool isValidModifierPlacement(int target, const ModifierCard* modifier) const;
    // postcondition: checks if you can place the card on the node 
    // returns true if you can and false if you cannot

    void attachCards();
    //postcondition: attaches all the temporarily attached cards onto the bridge

    // ---- Movement / traversal (Moving Phase) ----
    void startMove();
    // postcondtion: moves player one node forward to start the move phase
    // it will also check the nodes to calculate how many more times the player
    // needs to move 

    // ---- Text UI support ----
    void printBridge() const;
    // postcondition: prints the bridge

private:
    Node* player1Castle;
    Node* player2Castle;
    bool areAdjacent(const Node* a, const Node* b) const;
    void relinkAround(Node* target);  
};

#endif // BRIDGE_H