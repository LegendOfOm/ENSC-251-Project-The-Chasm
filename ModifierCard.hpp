#ifndef MODIFIERCARD_HPP
#define MODIFIERCARD_HPP

#include "Card.hpp"

class ModifierCard : public Card
{
public:
    // postcondition: creates a ModifierCard with a default name
    ModifierCard();

    // precondition: cardName should contain the name of the modifier card
    // postcondition: creates a ModifierCard and initializes its name
    ModifierCard(const std::string& cardName);

    // precondition: takes in the current movement amount
    // postcondition: returns the movement amount after applying
    // the modifier card's effect
    virtual int getModifiedAmount(int);

    // postcondition: creates and returns a dynamically allocated copy
    // of the current ModifierCard
    virtual ModifierCard* clone() const;
    
    // postcondition: returns a string representation of the ModifierCard
    // used when displaying the card in the player's hand
    virtual std::string Handoutput() const;
};

#endif
