#include "DiceNode.hpp"
#include "rng.hpp"

DiceNode::DiceNode()
    : NodeCard("Dice Node")
{
}

int DiceNode::rollDice() const
{
    const int results[6] = {4, 2, 1, -1, -2, -4};
    std::uniform_int_distribution<int> distrib(0, 5);
    int index = distrib(rng::randomNumber);

    return results[index];
}

std::string DiceNode::output() const
{
    return "Dice   ";
}

DiceNode* DiceNode::clone() const
{
    return new DiceNode(*this);
}

std::string DiceNode::Handoutput() const
{
    return "Dice   ";
}