#ifndef BOOSTER_HPP
#define BOOSTER_HPP

#include "ModifierCard.hpp"

class Booster : public ModifierCard
{
private:
    int boostAmount;

public:
    Booster(int amount);
    int getBoostAmount() const;
    virtual int getModifiedAmount(int originalMovment);
    std::string Handoutput() const override;
    virtual Booster* clone() const;
};

#endif