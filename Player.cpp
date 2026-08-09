#include "Player.hpp"

// Constructor for Player.
// Places the player at startingCastle (both current and castle node), stores the player id, and starts with an empty hand.
Player::Player(int playerId, Node* startingCastle)
    : playerId(playerId),
      currentNode(startingCastle),
      castleNode(startingCastle),
      handSize(0)
{
    for (int i = 0; i < MAX_HAND_SIZE; i++) {
        hand[i] = nullptr;
    }
}

Player::~Player() {
    // Nothing to delete, a player only observes its nodes and cards: the Bridge owns
    // every Node and the Deck owns every Card, and each deletes its own in its
    // destructor. hand is a fixed size member array  not an allocation.
}

// Copy constructor for Player. Copies other's position, hand, and activation state; card and node pointers are copied as, is shared, not duplicated
Player::Player(const Player& other)
    : playerId(other.playerId),
      currentNode(other.currentNode),
      castleNode(other.castleNode),
      handSize(other.handSize),
      activatedCardsThisTurn(other.activatedCardsThisTurn)
{
    // Shallow copy on purpose: copy observes the same nodes and cards as other.
    for (int i = 0; i < handSize; i++) {
        hand[i] = other.hand[i];
    }
    for (int i = handSize; i < MAX_HAND_SIZE; i++) {
        hand[i] = nullptr;
    }
}

// Assigns other's values to this Player, stopping self assignment.
Player& Player::operator=(const Player& other) {
    if (this == &other) {
        return *this; 
    }

    playerId = other.playerId;
    currentNode = other.currentNode;
    castleNode = other.castleNode;
    handSize = other.handSize;
    activatedCardsThisTurn = other.activatedCardsThisTurn;

    for (int i = 0; i < handSize; i++) {
        hand[i] = other.hand[i];
    }
    for (int i = handSize; i < MAX_HAND_SIZE; i++) {
        hand[i] = nullptr;
    }

    return *this;
}

// Identity/Position

// Returns this player's id
int Player::getPlayerId() const {
    return playerId;
}

// Returns the node this player is currently standing on
Node* Player::getCurrentNode() const {
    return currentNode;
}

// Moves the player to the given node
void Player::setCurrentNode(Node* node) {
    currentNode = node;
}

// Returns the player's castle node
Node* Player::getCastleNode() const {
    return castleNode;
}

// Drawing Phase

// Adds a card to the end of the hand, will fail if card is null or the hand is full
bool Player::addCardToHand(Card* card) {
    if (card == nullptr || isHandFull()) {
        return false;
    }
    hand[handSize] = card;
    handSize++;
    return true;
}

// Removes the given card from the hand, shifting later cards left to close the gap, returns false if the card isn't in the hand
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

// Returns the number of cards currently in hand
int Player::getHandSize() const {
    return handSize;
}

// Returns the card at index, or nullptr if index is out of range
Card* Player::getCardInHand(int index) const {
    if (index < 0 || index >= handSize) {
        return nullptr; 
    }
    return hand[index];
}

// Returns whether the hand is at MAX_HAND_SIZE
bool Player::isHandFull() const {
    return handSize >= MAX_HAND_SIZE;
}

// Moving Phase

// Returns whether node has already been activated by this player this turn
bool Player::hasActivated(const Node* node) const {
    for (size_t i = 0; i < activatedCardsThisTurn.size(); i++) {
        if (activatedCardsThisTurn[i] == node) {
            return true;
        }
    }
    return false;
}

// Marks landedNode as activated for this turn, if not already marked
void Player::markActivated(Node* landedNode) {
    if (!hasActivated(landedNode)) {
        activatedCardsThisTurn.push_back(landedNode);
    }
}

// Clears the activation list at the start of a new turn
void Player::resetActivatedCardsForNewTurn() {
    activatedCardsThisTurn.clear();
}