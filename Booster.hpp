#ifndef BOOSTER_HPP
#define BOOSTER_HPP

#include "ModifierCard.hpp"

class Booster : public ModifierCard
{
private:
    // Stores the amount of movement that this Booster card adds
    int boostAmount;

public:
    Booster(int amount);
    // precondition: amount should be the amount of movement the Booster will add
    // postcondition: creates a Booster card and initializes boostAmount to amount

    int getBoostAmount() const;
    // postcondition: returns the boost amount of this Booster card

    virtual int getModifiedAmount(int originalMovment);
    // precondition: originalMovement is the movement amount before the Booster is applied
    // postcondition: returns the movement amount after adding boostAmount

    std::string Handoutput() const override;
    // postcondition: returns a string representation of the Booster card
    // used when displaying the card in the player's hand

    virtual Booster* clone() const;
    // postcondition: creates and returns a dynamically allocated copy
    // of the current Booster card
};

#endif