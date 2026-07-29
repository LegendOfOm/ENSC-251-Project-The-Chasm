#ifndef BRIDGE_H
#define BRIDGE_H

#include <string>
#include "Card.hpp"
#include "ModifierCard.hpp"
#include "NodeCard.hpp"

struct ModifierStrand {
    ModifierStrand* next = nullptr;

    ModifierCard* modifierCard = nullptr;
};

struct Node {
    Node* left = nullptr;
    Node* right = nullptr;

    NodeCard* nodeCard = nullptr;

    ModifierStrand* beginningOfStrand = nullptr;

    bool isPlayer1Castle = false;
    bool isPlayer2Castle = false;
};

class Bridge {
public:
    Bridge();
    // constructor
    // postcondition: Initializes the bridge for the start of the game 
    //                Defaults to 12 nodes INCLUDING the castle nodes  
    //                creates a doubly linked list and intializes castle nodes
    
    ~Bridge();
    // destructor
    // postcondition: deletes the bridge

    Bridge(const Bridge&);
    // copy constructor 
    // postcondition: creates a deep copy of the bridge 

    Bridge& operator=(const Bridge& other);

    // managing nodes
    bool insertCard(const int& leftNode, const int& rightNode, NodeCard* nodeCard);
    //postcondition: attaches all the temporarily attached cards onto the bridge

    bool removeNode(const int& targetNode);
    // postcondition: removes a node at targeNode index
    
    bool attachModifierCard(const int& targetNode, ModifierCard* modifier);
    // postcondition: attached a modifier card at the end of the card strand at a specific node 
    
    // validation
    bool isValidNodePlacement(const int& leftNode, const int& rightNode) const;
    // postcondition: checks if a node can be created between the two nodes 
    // returns true if you can and false if you cannot
    
    bool isValidNode(const int& target) const; 
    // postcondition: checks the node is within the bounds of the bridge
    // since castle nodes cannot be edited, those nodes will return FALSE 
    // returns true if it is false if it isnt

    int castleNodeCheck(const Node* node) const;
    // postcondition: checks whether or not the node is a castle node or not. 
    // returns 0 for false, 1 for player 1 castle, 2 for player 2 castle

    // ---- Text UI support ----
    void printBridge() const;
    // postcondition: prints the bridge

private:
    Node* player1Castle;
    Node* player2Castle;

    int numberOfNodes;

    void modifierStrandDeleter(ModifierStrand*);
    Node* travelToNode(int);

    void clear();
    void copyFrom(const Bridge& other);
};

#endif // BRIDGE_H