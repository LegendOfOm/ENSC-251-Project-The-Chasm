#include "Multiplier.hpp"

Multiplier::Multiplier(int amount): ModifierCard("Multiplier"), multiplierAmount(amount){}

int Multiplier::getMultiplierAmount() const
{
    return multiplierAmount;
}