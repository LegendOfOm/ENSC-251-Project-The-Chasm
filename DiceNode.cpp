#include "DiceNode.hpp"
#include <random>
#include <chrono>

DiceNode::DiceNode()
    : NodeCard("Dice Node")
{
}

int DiceNode::rollDice() const
{
    const int results[6] = {4, 2, 1, -1, -2, -4};

    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distrib(0, 5);
    int index = distrib(rng);

    return results[index];
}

std::string DiceNode::output() const
{
    return "[Dice   ]";
}

DiceNode* DiceNode::clone() const
{
    return new DiceNode(*this);
}