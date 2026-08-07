#include "NodeCard.hpp"

NodeCard::NodeCard(const std::string& cardName): Card(cardName), movementAmount(0){}

int NodeCard::getMovementAmount() const
{
    return movementAmount;
}

std::string NodeCard::output() const
{
    return "[Norm   ]";
}

NodeCard* NodeCard::clone() const
{
    return new NodeCard(*this);
}