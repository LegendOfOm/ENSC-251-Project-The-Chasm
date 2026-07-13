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
};

#endif