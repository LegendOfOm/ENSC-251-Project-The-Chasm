#ifndef MULTIPLIER_HPP
#define MULTIPLIER_HPP

#include "ModifierCard.hpp"

class Multiplier : public ModifierCard
{
private:
    int multiplierAmount;

public:
    Multiplier(int amount);
    int getMultiplierAmount() const;
    virtual int getModifiedAmount(int originalMovment);
    std::string Handoutput() const override;
    virtual Multiplier* clone() const;
};

#endif