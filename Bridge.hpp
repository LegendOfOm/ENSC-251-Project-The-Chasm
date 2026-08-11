#ifndef BRIDGE_H
#define BRIDGE_H

#include <string>
#include "card/Card.hpp"
#include "card/ModifierCard.hpp"
#include "card/NodeCard.hpp"

struct ModifierStrand {
    ModifierStrand* next = nullptr;

    ModifierCard* modifierCard = nullptr;
};

struct Node {
    Node* left = nullptr;
    Node* right = nullptr;

    int movementAmount = 0;

    NodeCard* nodeCard = nullptr;

    ModifierStrand* beginningOfStrand = nullptr;

    bool isPlayer1Castle = false;
    bool isPlayer2Castle = false;
};

class Bridge {
public:
    // postcondition: Initializes the bridge for the start of the game 
    // Default is 12 nodes INCLUDING the castle nodes 
    // creates a doubly linked list and intializes castle nodes
    // Every node will always have a card. 
    Bridge();
    
    // postcondition: deletes the bridge
    ~Bridge();

    // postcondition: creates a deep copy of the bridge 
    Bridge(const Bridge& other);

    // poscondition: assigns the right side to the left side. 
    Bridge& operator=(const Bridge& other);

    // managing nodes
        
    // postcondition: returns the number of nodes on the bridge including the castle nodes
    int getNumberOfNodes() const;

    // precondition: leftNode and rightNode must be next to each other
    // postcondition: attches the nodeCard onto the bridge inbetween the two nodes
    // attches to the left side first. 
    // It will also update movement amount. exception: portal and dice
    // portal and dice will keep the movement amount at 0.
    bool insertCard(const int& leftNode, const int& rightNode, NodeCard* nodeCard);
    
    // precondition: node must be a valid node
    // postcondition: removes a node at targeNode index
    bool removeNode(const int& targetNode);
    
    // precondition: the targetNode must be a valid node
    // postcondition: attaches a modifier card at the end of the card strand at the targetNode. 
    // It will also update movement amount. 
    bool attachModifierCard(const int& targetNode, ModifierCard* modifier);
    
    // validation
    
    // postcondition: checks if a node can be created between the two nodes 
    // returns true if you can
    // returns false if you cannot
    bool isValidNodePlacement(const int& leftNode, const int& rightNode) const;
    
    // postcondition: checks the node is within the bounds of the bridge
    // since castle nodes cannot be edited, those nodes will return FALSE 
    // returns true if it is within the bounds 
    // returns false if it if not within the bounds
    bool isValidNode(const int& target) const; 
    
    // postcondition: checks whether or not the node is a castle node or not. 
    // returns 0 for false, 1 for player 1 castle, 2 for player 2 castle
    int castleNodeCheck(const Node* node) const;
    
    // postcondition: returns the castle node of player 1
    Node* getPlayer1Castle() const;

    // postcondition: returns the castle node of player 2
    Node* getPlayer2Castle() const;

    // ---- Text UI support ----

    // postcondition: returns movement amount
    int getMovementOnBridge(const Node* targetNode) const;

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