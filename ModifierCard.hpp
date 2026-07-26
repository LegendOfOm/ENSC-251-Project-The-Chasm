#ifndef MODIFIERCARD_HPP
#define MODIFIERCARD_HPP

#include "Card.hpp"

class ModifierCard : public Card
{
public:
    ModifierCard();
    ModifierCard(const std::string& cardName);
};

#endif
