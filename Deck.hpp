#ifndef DECK_HPP
#define DECK_HPP
#include <string>

class Card;

class Deck
{
public:
    // postcondition: Initializes the deck for the start of the game
    Deck();

    // postcondition: deletes the deck
    ~Deck();

    // postcondition: draws a card from the deck and without returning it
    Card* drawCard();

    // postcondition: returns the number of cards in the deck
    int getDeckSize() const;

    // postcondition: returns the card at the specified index in the deck
    Card* getCard(int index) const;

    // postcondition: returns true if the deck is empty, false otherwise
    bool isEmpty() const;

    // postcondition: generates the starting deck for the game
    void generateStartingDeck();

    // postcondition: generates a deck based on the current state of the game
    void generateDeckForState(int bridgeSize, int player1NodeIndex, int player2NodeIndex);

    // postcondition: shuffles the deck of cards
    void shuffleDeck();

    // postcondition: returns a string representation of the deck
    std::string outputDeck() const;

    // postcondition: draws a card from the deck at the specified index without returning it  
    Card* drawCardAtIndex(int index);
    
private:
    static const int MAX_DECK_SIZE = 50;
    Card* cards[MAX_DECK_SIZE];
    int deckSize;
    int deckNumber;
    double u_previous;

    // postcondition: adds a card to the deck and returns true if successful, false if the deck is full
    bool addCard(Card* card);

    // postcondition: Deletes all cards in the deck and sets deckSize to 0
    void clearDeck();

    // postcondition: return the tier of the bridge state
    void buildTier(int tier);
};
#endif