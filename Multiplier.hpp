#ifndef MULTIPLIER_HPP
#define MULTIPLIER_HPP

#include "ModifierCard.hpp"

class Multiplier : public ModifierCard
{
private:
    // Stores the value that the movement amount will be multiplied by
    int multiplierAmount;

public:
    // precondition: amount should be the multiplier value
    // postcondition: creates a Multiplier card and initializes
    // multiplierAmount to amount
    Multiplier(int amount);

    // postcondition: returns the multiplier amount of this card
    int getMultiplierAmount() const;

    // precondition: originalMovment is the movement amount before
    // the Multiplier card is applied
    // postcondition: returns the movement amount after multiplying
    // it by multiplierAmount
    virtual int getModifiedAmount(int originalMovment);

    // postcondition: returns a string representation of the Multiplier card
    // used when displaying the card in the player's hand
    std::string Handoutput() const override;

    // postcondition: creates and returns a dynamically allocated copy
    // of the current Multiplier card
    virtual Multiplier* clone() const;
};

#endif