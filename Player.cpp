#include "Player.hpp"

// Constructor/Destructor
Player::Player(int playerId, Node* startingCastle)
    : playerId(playerId),
      currentNode(startingCastle),
      castleNode(startingCastle),
      handSize(0),
      won(false)
{
    for (int i = 0; i < MAX_HAND_SIZE; i++) {
        hand[i] = nullptr;
    }
}

Player::~Player() {
}

Player::Player(const Player& other)
    : playerId(other.playerId),
      currentNode(other.currentNode),
      castleNode(other.castleNode),
      handSize(other.handSize),
      won(other.won),
      activatedCardsThisTurn(other.activatedCardsThisTurn) 
{
    for (int i = 0; i < MAX_HAND_SIZE; i++) {
        hand[i] = other.hand[i]; 
    }
}

Player& Player::operator=(const Player& other) {
    if (this == &other) {
        return *this; 
    }

    playerId = other.playerId;
    currentNode = other.currentNode;
    castleNode = other.castleNode;
    handSize = other.handSize;
    won = other.won;
    activatedCardsThisTurn = other.activatedCardsThisTurn;

    for (int i = 0; i < MAX_HAND_SIZE; i++) {
        hand[i] = other.hand[i]; 
    }

    return *this;
}

// Identity/Position
int Player::getPlayerId() const {
    return playerId;
}

Node* Player::getCurrentNode() const {
    return currentNode;
}

void Player::setCurrentNode(Node* node) {
    currentNode = node;
}

Node* Player::getCastleNode() const {
    return castleNode;
}

// Drawing Phase
bool Player::addCardToHand(Card* card) {
    if (card == nullptr || isHandFull()) {
        return false;
    }
    hand[handSize] = card;
    handSize++;
    return true;
}

bool Player::removeCardFromHand(Card* card) {
    for (int i = 0; i < handSize; i++) {
        if (hand[i] == card) {
            for (int j = i; j < handSize - 1; j++) {
                hand[j] = hand[j + 1];
            }
            hand[handSize - 1] = nullptr;
            handSize--;
            return true;
        }
    }
    return false; 
}

int Player::getHandSize() const {
    return handSize;
}

Card* Player::getCardInHand(int index) const {
    if (index < 0 || index >= handSize) {
        return nullptr; 
    }
    return hand[index];
}

bool Player::isHandFull() const {
    return handSize >= MAX_HAND_SIZE;
}

// Moving Phase

bool Player::hasActivated(const Node* node) const {
    for (size_t i = 0; i < activatedCardsThisTurn.size(); i++) {
        if (activatedCardsThisTurn[i] == node) {
            return true;
        }
    }
    return false;
}

void Player::markActivated(Node* landedNode) {
    if (!hasActivated(landedNode)) {
        activatedCardsThisTurn.push_back(landedNode);
    }
}

void Player::resetActivatedCardsForNewTurn() {
    activatedCardsThisTurn.clear();
}

// Win Condition

bool Player::hasWon() const {
    return won;
}

void Player::setWon(bool wonStatus) {
    won = wonStatus;
}