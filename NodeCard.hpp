#ifndef NODECARD_HPP
#define NODECARD_HPP

#include "Card.hpp"

class NodeCard : public Card
{
public:
    NodeCard(const std::string& cardName = "Normal Node");
    NodeCard(const std::string& cardName);
    virtual int getMovementAmount() const;
    std::string output() const override;
};

#endif