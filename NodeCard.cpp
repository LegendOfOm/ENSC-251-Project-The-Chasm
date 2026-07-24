#include "NodeCard.hpp"

NodeCard::NodeCard(const std::string& cardName): Card(cardName){}

int NodeCard::getMovementAmount() const
{
    return 0;
}

std::string NodeCard::output() const
{
    return "[Norm  ]";
}