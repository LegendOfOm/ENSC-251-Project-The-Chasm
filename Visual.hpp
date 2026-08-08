#ifndef VISUAL_HPP
#define VISUAL_HPP

#include "Bridge.hpp"
#include "Player.hpp"

class Visual
{
public:
    Visual();

    // postcondition:
    // Prints every node from player 1's castle to player 2's castle.
    void printBridge(const Bridge& bridge, const Player& player1, const Player& player2) const;
    void printHands(const Player& player1,const Player& player2) const;

private:
    // precondition:
    // node points to a valid bridge node.
    // postcondition:
    // Prints the NodeCard stored inside the node.
    void printNode(const Node* node, int index, const Player& player1, const Player& player2, const Bridge& bridge) const;

    // precondition:
    // strand points to the beginning of a modifier strand or is nullptr.
    // postcondition:
    // Prints the modifier information attached to a node.
    void printHandHelper(const Player& player) const;
};

#endif