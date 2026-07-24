#include "Card.hpp"

Card::Card(const std::string& cardName)
{
    name = cardName;
}

Card::~Card()
{
}

std::string Card::getName() const
{
    return name;
}

std::string Card::output() const
{
    return name;
}