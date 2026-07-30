#ifndef NODECARD_HPP
#define NODECARD_HPP

#include "Card.hpp"

class NodeCard : public Card
{
private:
    int movementAmount;
public:
    NodeCard(const std::string& cardName = "Normal Node");
    virtual int getMovementAmount() const;
    std::string output() const override;
    virtual NodeCard* clone() const;
};

#endif