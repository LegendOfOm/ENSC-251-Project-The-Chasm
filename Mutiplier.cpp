#include "Multiplier.hpp"

Multiplier::Multiplier(int amount): ModifierCard("Multiplier"), multiplierAmount(amount){}

int Multiplier::getMultiplierAmount() const
{
    return multiplierAmount;
}

Multiplier* Multiplier::clone() const
{
    return new Multiplier(*this);
}

int Multiplier::getModifiedAmount(int originalMovment) {
    return originalMovment * multiplierAmount;
}

std::string Multiplier::output() const
{
    return "Multiplier x" + std::to_string(multiplierAmount);
}
