#include "Booster.hpp"
#include <string>

Booster::Booster(int amount)
    : ModifierCard("Booster"), boostAmount(amount)
{
}

int Booster::getAmount() const
{
    return boostAmount;
}

std::string Booster::output() const
{
    return "[Bstr " + std::to_string(boostAmount) + "]";
}