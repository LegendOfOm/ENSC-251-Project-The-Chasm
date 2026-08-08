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
        printNode(currentNode, index, player1, player2, bridge);
        if(currentNode->right != nullptr)
        {
            std::cout << " --- ";
        }
        currentNode = currentNode->right;
        if (index % 5 == 0) std::cout << std::endl;
        index++;
    }
}

void  Visual::printNode(const Node* node, int index, const Player& player1, const Player& player2, const Bridge& bridge) const
{
    if (node->isPlayer1Castle)
    {
        std::cout << "[P1's| Castle |]";
    }

    else if (node->isPlayer2Castle)
    {
        std::cout << "[P2's| Castle |]";
    }

    else
    {
        std::cout << "[|";
        if (player1.getCurrentNode() == node &&player2.getCurrentNode() == node)
        {
            std::cout << "12";
        }
        else if (player1.getCurrentNode() == node)
        {
            std::cout << "\033[34mP1\033[0m";
        }
        else if (player2.getCurrentNode() == node)
        {
            std::cout << "\033[31mP2\033[0m";
        }
        else
        {
            std::cout << "  ";
        }

        std::cout << "|";
        
        if (node->nodeCard != nullptr)
        {
            std::cout << node->nodeCard->output();
            std::cout << " ";
            int bridgeMovementNumber = bridge.getMovementOnBridge(node);
            if (bridgeMovementNumber < 10 && bridgeMovementNumber >= 0)
            {
                std::cout << bridgeMovementNumber << "  ";
            } else if ((bridgeMovementNumber < 0 && bridgeMovementNumber > -10) || (bridgeMovementNumber >= 10)) {
                std::cout << bridgeMovementNumber << " ";
            } else {
                std::cout << bridgeMovementNumber;
            }

        }
        else
        {
            std::cout << "Norm   ";
        }

        std::cout << "]";
    }
    if (index < 10)
    {
        std::cout << " " << index;
    }
    else
    {
        std::cout << index;
    }
}

void Visual::printHands(const Player& player1,const Player& player2) const
{
    printHandHelper(player1);
    printHandHelper(player2);
}

void Visual::printHandHelper(const Player& player) const
{   if (player.getPlayerId() == 1) {
        std::cout << "\033[34m"; // Blue color code for Player 1
    } else {
        std::cout << "\033[31m"; // Red color code for Player 2
    }
    std::cout << "P " << player.getPlayerId() << "\033[0m" << "'s Hand: ";
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
                std::cout << card->Handoutput();
                if (i < player.getHandSize() - 1)
                {
                    std::cout << ", ";
                }
            }
        }
    }
    std::cout << std::endl;
}