#include "BoosterNode.hpp"
#include <string>

BoostNode::BoostNode(int amount): NodeCard("Boost Node"), boostAmount(amount){}

int BoostNode::getMovementAmount() const
{
    return boostAmount;
}

std::string BoostNode::output() const
{
    const std::string colorCode = "\033[32m"; // Green color code
    const std::string resetCode = "\033[0m"; // Reset color code
    return colorCode + "[BNod+" + std::to_string(boostAmount) + "]" + resetCode;
}

BoostNode* BoostNode::clone() const
{
    return new BoostNode(*this);
}