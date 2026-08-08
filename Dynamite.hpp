#ifndef DYNAMITE_HPP
#define DYNAMITE_HPP

#include "ModifierCard.hpp"

class Dynamite : public ModifierCard
{
public:
    // postcondition: creates a Dynamite modifier card
    Dynamite();

    // postcondition: returns a string representation of the Dynamite card
    // used when displaying the card in the player's hand
    std::string Handoutput() const override;

    // postcondition: it doesnt have a value it only here nso iut doesnt break the code
    virtual int getModifiedAmount(int originalMovment);

};

#endif