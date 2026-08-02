#ifndef BRIDGEVISUAL_HPP
#define BRIDGEVISUAL_HPP

#include "Bridge.hpp"

class BridgeVisual
{
public:
    BridgeVisual();

    // postcondition:
    // Prints every node from player 1's castle to player 2's castle.
    void printBridge(const Bridge& bridge) const;

private:
    // precondition:
    // node points to a valid bridge node.
    // postcondition:
    // Prints the NodeCard stored inside the node.
    void printNode(const Node* node) const;

    // precondition:
    // strand points to the beginning of a modifier strand or is nullptr.
    // postcondition:
    // Prints the modifier information attached to a node.
    void printModifiers(const ModifierStrand* strand) const;
};

#endif