#include "Deck.hpp"
#include "Card.hpp"
#include <iostream>
#include <string>

Deck::Deck() : deckSize(0) {

}

Deck::~Deck() {
    clearDeck();
}

Card* Deck::drawCard() {
    if (deckSize == 0) {
        return nullptr; // Deck is empty
    }
    Card* drawnCard = cards[deckSize - 1];
    cards[deckSize - 1] = nullptr; // Remove the card from the deck
    deckSize--;
    return drawnCard;
}

Card* Deck::drawCardAtIndex(int index) {
    if (index < 0 || index >= deckSize) {
        return nullptr; // Invalid index
    }
    Card* drawnCard = cards[index];
    for (int i = index; i < deckSize - 1; ++i) {
        cards[i] = cards[i + 1]; // Shift cards down
    }
    cards[deckSize - 1] = nullptr; // Remove the last card
    deckSize--;
    return drawnCard;
}

void Deck::clearDeck() {
    for (int i = 0; i < deckSize; ++i) {
        delete cards[i];
        cards[i] = nullptr;
    }
    deckSize = 0;
}

Card* Deck::getCard(int index) const {
    if (index < 0 || index >= deckSize) {
        return nullptr; // Invalid index
    }
    return cards[index];
}

void Deck::generateStartingDeck() {
    clearDeck();
    // Example: Add cards to the deck (I didn't make the card generation logic so u gotta do it :D)
    for (int i = 0; i < MAX_DECK_SIZE; ++i) {
        addCard(new Card("Card " + std::to_string(i + 1)));
    }
}

void Deck::generateDeckForState(int bridgeSize, int player1NodeIndex, int player2NodeIndex) {
    clearDeck();
    //I kinda feel bad for leaving this and the card generation logic actually D; have fun tho :D
}

void Deck::shuffleDeck() {
    for (int i = 0; i < deckSize; ++i) {
        int randomIndex = rand() % deckSize;
        std::swap(cards[i], cards[randomIndex]);
    }
}

std::string Deck::outputDeck() const {
    std::string output;
    for (int i = 0; i < deckSize; ++i) {
        output += cards[i]->output() + "\n";
    }
    return output;
}

bool Deck::isEmpty() const {
    if (deckSize == 0) {
        return true; // Deck is empty
    } else {
        return false; // Deck is not empty
    }
}