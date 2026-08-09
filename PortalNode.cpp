#include "PortalNode.hpp"
// Constructor for PortalNode.
// Calls the NodeCard constructor with the name "Portal Node",
// stores the portal colour, and starts with no connected portal.
PortalNode::PortalNode(const std::string& portalColour)
    : NodeCard("Portal Node"),
      colour(portalColour),
      connectedPortal(nullptr)
{
}

// Returns the colour of this portal.
std::string PortalNode::getColour() const
{
    return colour;
}

// Connects this portal to another PortalNode.
void PortalNode::setConnectedPortal(PortalNode* portal)
{
    connectedPortal = portal;
}

// Returns the portal that this PortalNode is connected to.
PortalNode* PortalNode::getConnectedPortal() const
{
    return connectedPortal;
}

// Returns the text used to display the PortalNode on the bridge.
// ANSI colour codes are used so each portal colour appears differently.
std::string PortalNode::output() const
{
    if (colour == "Yellow")
    {
        return "\033[33mPortal\033[0m";
    }

    if (colour == "Blue")
    {
        return "\033[34mPortal\033[0m";
    }

    if (colour == "Purple")
    {
        return "\033[35mPortal\033[0m";
    }

    if (colour == "Orange")
    {
        return "\033[38;5;208mPortal\033[0m";
    }

    return "Portal";
}

// Creates and returns a new copy of the current PortalNode object.
PortalNode* PortalNode::clone() const
{
    return new PortalNode(*this);
}

// Returns the text used to display the PortalNode in the player's hand.
// The same colour formatting is used as the bridge display.
std::string PortalNode::Handoutput() const
{
    if (colour == "Yellow")
    {
        return "\033[33mPortal\033[0m";
    }

    if (colour == "Blue")
    {
        return "\033[34mPortal\033[0m";
    }

    if (colour == "Purple")
    {
        return "\033[35mPortal\033[0m";
    }

    if (colour == "Orange")
    {
        return "\033[38;5;208mPortal\033[0m";
    }

    return "Portal ";
}
