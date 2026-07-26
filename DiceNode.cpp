#include "DiceNode.hpp"
#include <cstdlib>

DiceNode::DiceNode()
    : NodeCard("Dice Node")
{
}

int DiceNode::rollDice() const
{
    const int results[6] = {4, 2, 1, -1, -2, -4};

    int index = std::rand() % 6;

    return results[index];
}

std::string DiceNode::output() const
{
    return "[Dice  ]";
}

DiceNode* DiceNode::clone() const
{
    return new DiceNode(*this);
}