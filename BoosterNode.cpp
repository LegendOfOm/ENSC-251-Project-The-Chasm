#include "BoostNode.hpp"
#include <string>

BoostNode::BoostNode(int amount): NodeCard("Boost Node"), boostAmount(amount){}

int BoostNode::getMovementAmount() const
{
    return boostAmount;
}

std::string BoostNode::output() const
{
    return "[BNod+" + std::to_string(boostAmount) + "]";
}