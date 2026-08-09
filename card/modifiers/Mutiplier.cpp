#include "Multiplier.hpp"

// Constructor for Multiplier.
Multiplier::Multiplier(int amount): ModifierCard("Multiplier"), multiplierAmount(amount){}

// Returns the value that movement will be multiplied by.
int Multiplier::getMultiplierAmount() const
{
    return multiplierAmount;
}

// Creates and returns a new copy of the current Multiplier object.
Multiplier* Multiplier::clone() const
{
    return new Multiplier(*this);
}

// Multiplies the original movement amount by the multiplier value
// and returns the new movement amount.
int Multiplier::getModifiedAmount(int originalMovment) {
    return originalMovment * multiplierAmount;
}

// Returns the text used to display the Multiplier card in the player's hand.
// Example: "Multiplier x2"
std::string Multiplier::Handoutput() const
{
    return "Multiplier x" + std::to_string(multiplierAmount);
}
