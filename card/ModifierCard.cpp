#include "ModifierCard.hpp"

// Default constructor for ModifierCard.
ModifierCard::ModifierCard():Card("Modifier"){}

// Constructor that allows a modifier card to have a specific name.
ModifierCard::ModifierCard(const std::string& cardName): Card(cardName){}

// Creates and returns a new copy of the current ModifierCard object.
ModifierCard* ModifierCard::clone() const
{
    return new ModifierCard(*this);
}

// It does not change the movement amount and simply returns the original value.
// Derived modifier cards can override this function with their own effect.
int ModifierCard::getModifiedAmount(int originalMovment) {
    return originalMovment;
}

// Returns the text used to display a basic ModifierCard in the player's hand.
std::string ModifierCard::Handoutput() const
{
    return "Modifier Card";
}