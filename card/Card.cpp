#include "Card.hpp"

// Constructor for Card.
Card::Card(const std::string& cardName)
{
    name = cardName;
}

// Virtual destructor for the Card class.
Card::~Card()
{
}

// Returns the name of the card.
std::string Card::getName() const
{
    return name;
}

// Creates and returns a new copy of the current Card object.
Card* Card::clone() const
{
    return new Card(*this);
}

// Returns the card name for displaying the card on the bridge.
std::string Card::output() const
{
    return name;
}

// Returns the card name for displaying the card in the player's hand.
std::string Card::Handoutput() const
{
    return name;
}