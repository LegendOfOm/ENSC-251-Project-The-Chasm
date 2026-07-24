#include "RecoilNode.hpp"
#include <string>

RecoilNode::RecoilNode(int amount): NodeCard("Recoil Node"), recoilAmount(amount){}

int RecoilNode::getMovementAmount() const
{
    return -recoilAmount;
}

std::string RecoilNode::output() const
{
    return "[RNod-" + std::to_string(recoilAmount) + "]";
}