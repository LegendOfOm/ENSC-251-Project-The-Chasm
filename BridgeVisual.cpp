#include "Bridge.hpp"
#include "BridgeVisual.hpp"
#include <iostream>

BridgeVisual::BridgeVisual()
{
}   

void BridgeVisual::printBridge(const Bridge& bridge) const
{
    Node* currentNode = bridge.getPlayer1Castle();

    while (currentNode != nullptr)
    {
        printNode(currentNode);
        printModifiers(currentNode->beginningOfStrand);
        if(currentNode->right != nullptr)
        {
            std::cout << " --- ";
        }
        currentNode = currentNode->right;
    }
}

void  BridgeVisual::printNode(const Node* node) const
{
    if ( node == nullptr)
    {
        return;
    }
    if (node->nodeCard != nullptr)
    {
        std::cout << node->nodeCard->output();
    }
    else
    {
        std::cout << "[Empty Node]";
    }
}

void BridgeVisual::printModifiers(const ModifierStrand* strand) const
{
    const ModifierStrand* current = strand;

    while (current != nullptr)
    {
        if (current->modifierCard != nullptr)
        {
            std::cout << " "
                      << current->modifierCard->getName();
        }

        current = current->next;
    }
}