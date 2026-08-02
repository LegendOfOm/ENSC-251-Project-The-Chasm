#include "RecoilNode.hpp"
#include <string>

RecoilNode::RecoilNode(int amount): NodeCard("Recoil Node"), recoilAmount(amount){}

int RecoilNode::getMovementAmount() const
{
    return -recoilAmount;
}

std::string RecoilNode::output() const
{
    const std::string colorCode = "\033[31m"; // Red color code
    const std::string resetCode = "\033[0m"; // Reset color code
    return colorCode + "[RNod-" + std::to_string(recoilAmount) + "]" + resetCode;
}

RecoilNode* RecoilNode::clone() const
{
    return new RecoilNode(*this);
}