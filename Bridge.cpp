#include "Bridge.hpp"
#include <iostream>
#include "NodeCard.hpp"

Bridge::Bridge() : numberOfNodes(12) {
    player1Castle = new Node;
    player1Castle->left = nullptr;
    player1Castle->isPlayer1Castle = true;
    
    Node* tempptr = player1Castle;
    for (int i = 0; i < 11; i++) {
        tempptr->right = new Node;
        tempptr->right->left = tempptr;
        tempptr = tempptr->left;
    }

    tempptr->right = nullptr;
    tempptr->isPlayer2Castle = true;
    player2Castle = tempptr; 
}

bool Bridge::insertCard(int leftNode, int rightNode, NodeCard* nodeCard) {
    Node* tempptrLeft = player1Castle; 
    for (int i = 1; i < leftNode; i++) {
        tempptrLeft = tempptrLeft->right;
    }
    
    Node* tempptrRight = tempptrLeft->right;
    
    tempptrLeft->right = new Node;
    tempptrRight->left = tempptrLeft->right;
    Node* newNode = tempptrLeft->right;
    newNode->nodeCard = nodeCard; 
    return true;
}

bool Bridge::removeNode(int targetNode) {
    Node* tempptr = player1Castle;
    for (int i = 1; i < targetNode; i++) {
        tempptr = tempptr->right;
    }
    Node* tempptrLeft = tempptr->left;
    Node* tempptrRight = tempptr->right;
    delete tempptr;

    tempptrLeft->right = tempptrRight;
    tempptrRight->left = tempptrLeft;
    return true;
}

int Bridge::castleNodeCheck(Node* node) {
    if (node->isPlayer1Castle) return 1;
    else if (node->isPlayer2Castle) return 2;
    return 0;
}

void Bridge::printBridge() const {
    Node* tempptr = player1Castle;
    std::cout << "[P1 Castle] - "; 
    int index = 1;
    while (tempptr->isPlayer2Castle == false) {
        std::cout << "[|" << index << "|" << "Node" << "] - ";
        index++; 
        tempptr = tempptr->right; 
    }
    std::cout << "[P2 Castle]"; 
}