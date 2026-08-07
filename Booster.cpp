#include "Booster.hpp"
#include <string>

Booster::Booster(int amount): ModifierCard("Booster"), boostAmount(amount){}

int Booster::getBoostAmount() const
{
    return boostAmount;
}

Booster* Booster::clone() const
{
    return new Booster(*this);
}

int Booster::getModifiedAmount(int originalMovment) {
    return originalMovment + boostAmount;
}
