#ifndef DICENODE_HPP
#define DICENODE_HPP

#include "NodeCard.hpp"

class DiceNode : public NodeCard
{
public:
    // postcondition: creates a DiceNode card
    DiceNode();

    // postcondition: rolls the dice and returns the movement amount
    // given by the DiceNode
    int rollDice() const;

    // postcondition: returns a string representation of the DiceNode
    // used when displaying the node on the bridge
    std::string output() const override;

    // postcondition: creates and returns a dynamically allocated copy
    // of the current DiceNode
    DiceNode* clone() const override;

    // postcondition: returns a string representation of the DiceNode
    // used when displaying the card in the player's hand
    std::string Handoutput() const override;
};

#endif