#include "RecoilNode.hpp"
#include <string>

// Constructor for RecoilNode.
RecoilNode::RecoilNode(int amount): NodeCard("Recoil Node"), recoilAmount(amount){}

// Returns the movement value of the RecoilNode.
int RecoilNode::getMovementAmount() const
{
    return -recoilAmount;
}

// Returns the text used to display the RecoilNode on the bridge.
// ANSI colour codes are used to display the node in red.
std::string RecoilNode::output() const
{
    const std::string colorCode = "\033[31m"; // Red color code
    const std::string resetCode = "\033[0m"; // Reset color code
    return colorCode + "RNod- " + resetCode;
}

// Creates and returns a new copy of the current RecoilNode object.
RecoilNode* RecoilNode::clone() const
{
    return new RecoilNode(*this);
}

// Returns the text used to display the RecoilNode in the player's hand.
// The recoil amount is included so the player can see how much movement is reduced.
std::string RecoilNode::Handoutput() const
{
    const std::string colorCode = "\033[31m"; // Red color code
    const std::string resetCode = "\033[0m"; // Reset color code
    std::string amount = std::to_string(recoilAmount);
    return colorCode + "RNod- " + amount + resetCode;
}