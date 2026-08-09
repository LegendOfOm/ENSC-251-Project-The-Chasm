#ifndef RECOILNODE_HPP
#define RECOILNODE_HPP

#include "NodeCard.hpp"

class RecoilNode : public NodeCard
{
private:
    // Stores the amount of movement that will be reduced
    int recoilAmount;

public:
    // precondition: amount should be the amount of movement to reduce
    // postcondition: creates a RecoilNode and initializes recoilAmount
    RecoilNode(int amount);

    // postcondition: returns the movement amount of this RecoilNode in negative 
    int getMovementAmount() const override;

    // postcondition: returns a string representation of the RecoilNode
    // used when displaying the node on the bridge
    std::string output() const override;

    // postcondition: creates and returns a dynamically allocated copy
    // of the current RecoilNode
    RecoilNode* clone() const override;

    // postcondition: returns a string representation of the RecoilNode
    // used when displaying the card in the player's hand
    std::string Handoutput() const override;
};

#endif