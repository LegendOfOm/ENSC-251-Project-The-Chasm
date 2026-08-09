#ifndef BOOSTNODE_HPP
#define BOOSTNODE_HPP

#include "../NodeCard.hpp"

class BoostNode : public NodeCard
{
private:
    // Stores the amount of movement this BoostNode gives
    int boostAmount;

public:
    // precondition: amount should be the movement amount of the BoostNode
    // postcondition: creates a BoostNode and initializes boostAmount to amount
    BoostNode(int amount);

    // postcondition: returns the movement amount of this BoostNode
    int getMovementAmount() const override;

    // postcondition: returns a string representation of the BoostNode
    // used when displaying the node on the bridge
    std::string output() const override;

    // postcondition: returns a string representation of the BoostNode
    // used when displaying the card in the player's hand
    std::string Handoutput() const override;

    // postcondition: creates and returns a dynamically allocated copy
    // of the current BoostNode
    BoostNode* clone() const override;
};

#endif