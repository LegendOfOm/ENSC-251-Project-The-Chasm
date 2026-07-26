#include "Bridge.hpp"
#include <iostream>

Bridge::Bridge() : numberOfNodes(12) {
    player1Castle = new Node;
    player1Castle->left = nullptr;
    player1Castle->isPlayer1Castle = true;
    
    Node* tempptr = player1Castle;
    for (int i = 0; i < 11; i++) {
        tempptr->right = new Node;
        tempptr->right->left = tempptr;
        tempptr = tempptr->right;
    }

    tempptr->right = nullptr;
    tempptr->isPlayer2Castle = true;
    player2Castle = tempptr; 
}

Bridge& Bridge::operator=(const Bridge& other) {
    if (this == &other) return *this;
    Node* myTemporaryptr;
    while (numberOfNodes > 0) {
        myTemporaryptr = player1Castle->right;
        if (player1Castle->beginningOfStrand != nullptr) modifierStrandDeleter(player1Castle->beginningOfStrand);
        delete player1Castle->nodeCard;
        delete player1Castle;
        player1Castle = myTemporaryptr;
        numberOfNodes--;
    }
    if (other.numberOfNodes == 0) {
        player1Castle = nullptr;
        player2Castle = nullptr;
        return *this;
    }

    player1Castle = new Node;
    player1Castle->left = nullptr;
    player1Castle->isPlayer1Castle = true;
    
    numberOfNodes = other.numberOfNodes;
    Node* otherTempptr = other.player1Castle->right;
    while (other.player1Castle != other.player2Castle) {
        myTemporaryptr->right = new Node;
        myTemporaryptr = myTemporaryptr->right;
        if (otherTempptr->nodeCard != nullptr);
    }
}

Bridge::~Bridge() {
    Node* myTemporaryptr;
    while (numberOfNodes > 0) {
        myTemporaryptr = player1Castle->right;
        if (player1Castle->beginningOfStrand != nullptr) modifierStrandDeleter(player1Castle->beginningOfStrand);
        delete player1Castle->nodeCard;
        delete player1Castle;
        player1Castle = myTemporaryptr;
        numberOfNodes--;
    }
    player1Castle = nullptr;
    player2Castle = nullptr;
}

void Bridge::modifierStrandDeleter(ModifierStrand* tempptr) {
    if (tempptr->next != nullptr) {
        modifierStrandDeleter(tempptr->next);
    }
    delete tempptr->modifierCard;
    delete tempptr;
}

bool Bridge::insertCard(const int& leftNode, const int& rightNode, NodeCard* newNodeCard) {
    if (!isValidNodePlacement(leftNode, rightNode)) return false;
    Node* tempptrLeft = travelToNode(leftNode);
    
    Node* tempptrRight = tempptrLeft->right;
    
    tempptrLeft->right = new Node;
    tempptrRight->left = tempptrLeft->right;
    Node* newNode = tempptrLeft->right;
    newNode->nodeCard = newNodeCard; 
    newNode->left = tempptrLeft;
    newNode->right = tempptrRight;
    numberOfNodes++;
    return true;
}

bool Bridge::attachModifierCard(const int& targetNode, ModifierCard* modifier) {
    if (!isValidNode(targetNode)) return false;
    Node* tempptr = travelToNode(targetNode);
    if (tempptr->beginningOfStrand == nullptr) {
        tempptr->beginningOfStrand = new ModifierStrand;
        tempptr->beginningOfStrand->modifierCard = modifier;
        tempptr->beginningOfStrand->next = nullptr;
        return true;
    }
    ModifierStrand* modifierTempptr = tempptr->beginningOfStrand;
    while (modifierTempptr->next != nullptr) {
        modifierTempptr = modifierTempptr->next;
    }
    modifierTempptr->next = new ModifierStrand;
    modifierTempptr = modifierTempptr->next;
    modifierTempptr->modifierCard = modifier;
    modifierTempptr->next = nullptr;
    return true;
}


bool Bridge::removeNode(const int& targetNode) {
    if (!isValidNode(targetNode)) return false;
    Node* tempptr = travelToNode(targetNode);
    Node* tempptrLeft = tempptr->left;
    Node* tempptrRight = tempptr->right;
    if (tempptr->beginningOfStrand != nullptr) modifierStrandDeleter(tempptr->beginningOfStrand);
    delete tempptr;

    tempptrLeft->right = tempptrRight;
    tempptrRight->left = tempptrLeft;
    numberOfNodes--;
    return true;
}

bool Bridge::isValidNodePlacement(const int& leftNode, const int& rightNode) const {
    if (rightNode - leftNode == 1 && leftNode > 0 && rightNode <= numberOfNodes) return true;
    return false;
}

bool Bridge::isValidNode(const int& target) const {
    if (target > 1 && target < numberOfNodes) return true;
    return false;
}


int Bridge::castleNodeCheck(const Node* node) const {
    if (node->isPlayer1Castle) return 1;
    else if (node->isPlayer2Castle) return 2;
    return 0;
}

void Bridge::printBridge() const {
    Node* tempptr = player1Castle;
    std::cout << "[P1 Castle] - "; 
    int index = 1;
    while (tempptr->isPlayer2Castle == false) {
        std::cout << "[|" << index << "|" << tempptr->nodeCard->output() << "] - ";
        index++; 
        tempptr = tempptr->right; 
    }
    std::cout << "[P2 Castle]"; 
}

Node* Bridge::travelToNode(int theNode) {
    Node* tempptr = player1Castle;
    for (int i = 1; i < theNode; i++) {
        tempptr = tempptr->right;
    }
    return tempptr;
}