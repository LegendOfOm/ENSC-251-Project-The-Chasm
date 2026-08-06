#include "Bridge.hpp"
#include "Visual.hpp"
#include <iostream>

Visual::Visual()
{
}   

void Visual::printBridge(const Bridge& bridge) const
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

void  Visual::printNode(const Node* node) const
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

void Visual::printModifiers(const ModifierStrand* strand) const
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

void Visual::printHands(const Player& player1,const Player& player2) const
{
    printHandHelper(player1);
    printHandHelper(player2);
}

void Visual::printHandHelper(const Player& player) const
{
    std::cout << "P " << player.getPlayerId() << "'s Hand: ";
    if (player.getHandSize() == 0)
    {
        std::cout << "[Empty Hand]";
    }
    else
    {
        for (int i = 0; i < player.getHandSize(); ++i)
        {
            Card* card = player.getCardInHand(i);
            if (card != nullptr)
            {
                std::cout << card->output();
                if (i < player.getHandSize() - 1)
                {
                    std::cout << ", ";
                }
            }
        }
    }
    std::cout << std::endl;
}