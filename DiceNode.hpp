#ifndef DICENODE_HPP
#define DICENODE_HPP

#include "NodeCard.hpp"

class DiceNode : public NodeCard
{
public:
    DiceNode();
    int rollDice() const;
    std::string output() const override;
    DiceNode* clone() const override;
};

#endif