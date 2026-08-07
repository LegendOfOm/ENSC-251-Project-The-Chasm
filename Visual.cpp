#include "Bridge.hpp"
#include "Visual.hpp"
#include <iostream>

Visual::Visual()
{
}   

void Visual::printBridge(const Bridge& bridge, const Player& player1, const Player& player2) const
{
    Node* currentNode = bridge.getPlayer1Castle();
    int index = 1;

    while (currentNode != nullptr)
    {
        printNode(currentNode, index, player1, player2);
        if(currentNode->right != nullptr)
        {
            std::cout << " --- ";
        }
        currentNode = currentNode->right;
        index++;
    }
}

void  Visual::printNode(const Node* node, int index, const Player& player1, const Player& player2) const
{
    if ( node == nullptr)
    {
        return;
    }
    bool player1Here = (node == player1.getCurrentNode());
    bool player2Here = (node == player2.getCurrentNode());\

    if (player1Here && player2Here)
    {
        std::cout << "P1 P2 ";
    }
    else if (player1Here)
    {
        std::cout << "P1 ";
    }
    else if (player2Here)
    {
        std::cout << "P2 ";
    }

    if (node->isPlayer1Castle)
    {
        std::cout << "[|Castle|]";
    }
    else if (node->isPlayer2Castle)
    {
        std::cout << "[|Castle|]";
    }

    else if (node->nodeCard != nullptr)
    {
        std::cout << node->nodeCard->output();
    }
    else
    {
        std::cout << "[Norm  ]";
    }
    std:: cout << index;
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