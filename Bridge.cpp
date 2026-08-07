#include "Bridge.hpp"
#include <iostream>
#include "NodeCard.hpp"
#include "DiceNode.hpp"
#include "PortalNode.hpp"
#include <typeinfo>

Bridge::Bridge() : numberOfNodes(12) {
    // initialize player1castle
    player1Castle = new Node;
    player1Castle->left = nullptr;
    player1Castle->isPlayer1Castle = true;
    player1Castle->nodeCard = new NodeCard;
    
    // initialize 10 nodes
    Node* tempptr = player1Castle;
    for (int i = 0; i < 11; i++) {
        tempptr->right = new Node;
        tempptr->right->left = tempptr;
        tempptr = tempptr->right;
        tempptr->nodeCard = new NodeCard;
    }

    // initialize player2castle
    tempptr->right = nullptr;
    tempptr->isPlayer2Castle = true;
    tempptr->nodeCard = new NodeCard;
    player2Castle = tempptr; 
}

Bridge::Bridge(const Bridge& other) {
    copyFrom(other);
}

Bridge& Bridge::operator=(const Bridge& other) {
    if (this == &other) return *this;
    clear();
    copyFrom(other);
    return *this;
}

Bridge::~Bridge() {
    clear();
}

void Bridge::clear() {
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

void Bridge::copyFrom(const Bridge& other) {
    player1Castle = nullptr;
    player2Castle = nullptr;
    numberOfNodes = 0;

    // if empty return 
    if (other.player1Castle == nullptr) return;
    
    
    numberOfNodes = other.numberOfNodes;
    player2Castle = nullptr;
    // initialize castle node
    player1Castle = new Node;
    player1Castle->left = nullptr;
    player1Castle->nodeCard = new NodeCard;
    player1Castle->isPlayer1Castle = true;
    Node* myTemporaryptr = player1Castle;
    ModifierStrand* myModptr;
    numberOfNodes = other.numberOfNodes;
    Node* otherTempptr = other.player1Castle->right;
    ModifierStrand* otherModptr = otherTempptr->beginningOfStrand;
    
    // start copying the rest
    while (otherTempptr != other.player2Castle) {
        myTemporaryptr->right = new Node;
        myTemporaryptr->right->left = myTemporaryptr;
        myTemporaryptr = myTemporaryptr->right;
        myTemporaryptr->movementAmount = otherTempptr->movementAmount;
        // copy the nodecards
        if (otherTempptr->nodeCard != nullptr) {
            myTemporaryptr->nodeCard = otherTempptr->nodeCard->clone();
        }
        // copy the first modifier
        if (otherModptr != nullptr) {
            myTemporaryptr->beginningOfStrand = new ModifierStrand;
            myModptr = myTemporaryptr->beginningOfStrand;
            myModptr->modifierCard = otherModptr->modifierCard->clone();
            otherModptr = otherModptr->next;
        }
        // copy the other modifiers 
        while (otherModptr != nullptr) {
            myModptr->next = new ModifierStrand;
            myModptr = myModptr->next;
            if (otherModptr->modifierCard != nullptr) myModptr->modifierCard = otherModptr->modifierCard->clone();
            otherModptr = otherModptr->next;
        }
        otherTempptr = otherTempptr->right;
        otherModptr = otherTempptr->beginningOfStrand;
    }
    // copy the player 2 castle node
    myTemporaryptr->right = new Node;
    myTemporaryptr->right->left = myTemporaryptr;
    myTemporaryptr = myTemporaryptr->right;
    myTemporaryptr->isPlayer2Castle = true;
    player2Castle = myTemporaryptr;
    player2Castle->nodeCard = new NodeCard;
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
    // find target node
    Node* tempptrLeft = travelToNode(leftNode);
    Node* tempptrRight = tempptrLeft->right;
    
    // initialize a new node
    tempptrLeft->right = new Node;
    tempptrRight->left = tempptrLeft->right;
    Node* newNode = tempptrLeft->right;
    newNode->nodeCard = newNodeCard; 
    newNode->left = tempptrLeft;
    newNode->right = tempptrRight;
    numberOfNodes++;
    
    // add the movement amount 
    if (typeid(*newNodeCard) == typeid(DiceNode) || typeid(*newNodeCard) == typeid(PortalNode)) {
        newNode->movementAmount = 0;
    } else {
        newNode->movementAmount = newNodeCard->getMovementAmount();
    }
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

    tempptr->movementAmount = modifier->getModifiedAmount(tempptr->movementAmount);
    return true;
}


bool Bridge::removeNode(const int& targetNode) {
    if (!isValidNode(targetNode)) return false;
    Node* tempptr = travelToNode(targetNode);
    Node* tempptrLeft = tempptr->left;
    Node* tempptrRight = tempptr->right;
    if (tempptr->beginningOfStrand != nullptr) modifierStrandDeleter(tempptr->beginningOfStrand);
    delete tempptr->nodeCard;
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

Node* Bridge::getPlayer1Castle() const {
        return player1Castle;
}

Node* Bridge::getPlayer2Castle() const {
        return player2Castle;
}

Node* Bridge::travelToNode(int theNode) {
    Node* tempptr = player1Castle;
    for (int i = 1; i < theNode; i++) {
        tempptr = tempptr->right;
    }
    return tempptr;
}

int Bridge::getMovementAmount(Node* targetNode) {
    return targetNode->movementAmount;   
}