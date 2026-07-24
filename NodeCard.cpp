#include "NodeCard.hpp"

NodeCard::NodeCard(const std::string& cardName)
    : Card(cardName)
{
}

std::string NodeCard::output() const
{
    return "[Norm  ]";
}