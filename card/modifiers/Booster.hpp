#ifndef BOOSTER_HPP
#define BOOSTER_HPP

#include "../ModifierCard.hpp"

class Booster : public ModifierCard
{
private:
    // Stores the amount of movement that this Booster card adds
    int boostAmount;

public:
    // precondition: amount should be the amount of movement the Booster will add
    // postcondition: creates a Booster card and initializes boostAmount to amount
    Booster(int amount);

    // postcondition: returns the boost amount of this Booster card
    int getBoostAmount() const;

    // precondition: originalMovement is the movement amount before the Booster is applied
    // postcondition: returns the movement amount after adding boostAmount
    virtual int getModifiedAmount(int originalMovment);

    // postcondition: returns a string representation of the Booster card
    // used when displaying the card in the player's hand
    std::string Handoutput() const override;

    // postcondition: creates and returns a dynamically allocated copy
    // of the current Booster card
    virtual Booster* clone() const;
};

#endif