#include "DiceNode.hpp"
#include "rng.hpp"

// Constructor for DiceNode.
DiceNode::DiceNode()
    : NodeCard("Dice Node")
{
}

// Rolls the dice by randomly selecting one of the possible movement values.
int DiceNode::rollDice() const
{
    const int results[6] = {4, 2, 1, -1, -2, -4};
    std::uniform_int_distribution<int> distrib(0, 5);
    int index = distrib(rng::randomNumber);

    return results[index];
}

// Returns the text used to display the DiceNode on the bridge.
std::string DiceNode::output() const
{
    return "Dice  ";
}

// Creates and returns a new copy of the current DiceNode object.
DiceNode* DiceNode::clone() const
{
    return new DiceNode(*this);
}

// Returns the text used to display the DiceNode in the player's hand.
std::string DiceNode::Handoutput() const
{
    return "Dice   ";
}