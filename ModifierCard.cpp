#include "ModifierCard.hpp"
ModifierCard::ModifierCard():Card("Modifier"){}
ModifierCard::ModifierCard(const std::string& cardName): Card(cardName){}

ModifierCard* ModifierCard::clone() const
{
    return new ModifierCard(*this);
}

int ModifierCard::getModifiedAmount(int originalMovment) {
    return originalMovment;
}

std::string ModifierCard::output() const
{
    return "Modifier Card";
}