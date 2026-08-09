#ifndef PORTALNODE_HPP
#define PORTALNODE_HPP

#include "../NodeCard.hpp"

class PortalNode : public NodeCard
{
private:
    // Stores the colour used to identify the portal
    std::string colour;

    // Points to the other portal that this portal is connected to
    PortalNode* connectedPortal;
public:
    // precondition: portalColour should contain the colour of the portal
    // postcondition: creates a PortalNode with the given colour
    PortalNode(const std::string& portalColour);

    // postcondition: returns the colour of this PortalNode
    std::string getColour() const;

    // precondition: portal should point to another PortalNode
    // postcondition: connects this portal to the given portal
    void setConnectedPortal(PortalNode* portal);

    // postcondition: returns a pointer to the portal connected to this PortalNode
    PortalNode* getConnectedPortal() const;

    // postcondition: returns a string representation of the PortalNode
    // used when displaying the card in the player's hand
    std::string Handoutput() const override;

    // postcondition: returns a string representation of the PortalNode
    // used when displaying the node on the bridge
    std::string output() const override;

    // postcondition: creates and returns a dynamically allocated copy
    // of the current PortalNode
    PortalNode* clone() const override;
};

#endif