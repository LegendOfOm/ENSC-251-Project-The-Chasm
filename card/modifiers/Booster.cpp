#include "Booster.hpp"
#include <string>

// Constructor for Booster.
Booster::Booster(int amount): ModifierCard("Booster"), boostAmount(amount){}

// Returns the amount of movement this Booster adds.
int Booster::getBoostAmount() const
{
    return boostAmount;
}

// Creates a new copy of the current Booster object.
Booster* Booster::clone() const
{
    return new Booster(*this);
}

// Adds the Booster's boostAmount to the original movement
int Booster::getModifiedAmount(int originalMovment) {
    return originalMovment + boostAmount;
}

// Creates the text used to display the Booster card in the player's hand.
std::string Booster::Handoutput() const
{
    return "Booster +" + std::to_string(boostAmount);
}