#ifndef NODECARD_HPP
#define NODECARD_HPP

#include "Card.hpp"

class NodeCard : public Card
{
private:
    // Stores the movement amount given by this node
    int movementAmount;

public:
    // precondition: cardName should contain the name of the node card
    // postcondition: creates a NodeCard with the given name
    // default name is "Normal Node"
    NodeCard(const std::string& cardName = "Normal Node");

    // postcondition: returns the movement amount of this NodeCard
    virtual int getMovementAmount() const;

    // postcondition: returns a string representation of the NodeCard
    // used when displaying the node on the bridge
    std::string output() const override;

    // postcondition: creates and returns a dynamically allocated copy
    // of the current NodeCard
    virtual NodeCard* clone() const;

    // postcondition: returns a string representation of the NodeCard
    // used when displaying the card in the player's hand
    std::string Handoutput() const override;
};

#endif