#ifndef RECOILER_HPP
#define RECOILER_HPP

#include "ModifierCard.hpp"

class Recoiler : public ModifierCard
{
private:
    // Stores the amount of movement that will be reduced
    int recoilAmount;

public:
    // precondition: amount should be the amount of movement to reduce
    // postcondition: creates a Recoiler card and initializes recoilAmount
    Recoiler(int amount);

    // postcondition: returns the recoil amount of this card in negative
    int getRecoilAmount() const;

    // postcondition: creates and returns a dynamically allocated copy
    // of the current Recoiler card
    virtual Recoiler* clone() const;

    // precondition: originalMovment is the movement amount before
    // the Recoiler card is applied
    virtual int getModifiedAmount(int originalMovment);
    
    // postcondition: returns a string representation of the Recoiler card
    // used when displaying the card in the player's hand
    std::string Handoutput() const override;
};

#endif