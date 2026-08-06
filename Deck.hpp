#ifndef DECK_HPP
#define DECK_HPP
#include <string>

class Card;

class Deck
{
public:
    Deck();
    // postcondition: Initializes the deck for the start of the game

    ~Deck();
    // postcondition: deletes the deck

    Card* drawCard();
    // postcondition: draws a card from the deck and without returning it

    int getDeckSize() const;
    // postcondition: returns the number of cards in the deck

    Card* getCard(int index) const;
    // postcondition: returns the card at the specified index in the deck

    bool isEmpty() const;
    // postcondition: returns true if the deck is empty, false otherwise

    void generateStartingDeck();
    // postcondition: generates the starting deck for the game

    void generateDeckForState(int bridgeSize, int player1NodeIndex, int player2NodeIndex);
    // postcondition: generates a deck based on the current state of the game

    void shuffleDeck();
    // postcondition: shuffles the deck of cards

    std::string outputDeck() const;
    // postcondition: returns a string representation of the deck

    Card* drawCardAtIndex(int index);
    // postcondition: draws a card from the deck at the specified index without returning it  
    
private:
    static const int MAX_DECK_SIZE = 50;
    Card* cards[MAX_DECK_SIZE];
    int deckSize;
    int deckNumber;
    double u_previous;

    bool addCard(Card* card);
    // postcondition: adds a card to the deck and returns true if successful, false if the deck is full

    void clearDeck();
    // postcondition: Deletes all cards in the deck and sets deckSize to 0

    void buildTier(int tier);


};
#endif