#ifndef MODIFIERCARD_HPP
#define MODIFIERCARD_HPP

#include "Card.hpp"

class ModifierCard : public Card
{
public:
    ModifierCard(const std::string& cardName);
    std::string output() const override;
};

#endif
