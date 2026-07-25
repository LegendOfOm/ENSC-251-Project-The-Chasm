#include "PortalNode.hpp"

PortalNode::PortalNode(const std::string& portalColour)
    : NodeCard("Portal Node"),
      colour(portalColour),
      connectedPortal(nullptr)
{
}

std::string PortalNode::getColour() const
{
    return colour;
}

void PortalNode::setConnectedPortal(PortalNode* portal)
{
    connectedPortal = portal;
}

PortalNode* PortalNode::getConnectedPortal() const
{
    return connectedPortal;
}

std::string PortalNode::output() const
{
    if (colour == "Red")
    {
        return "[Red P ]";
    }

    if (colour == "Blue")
    {
        return "[BlueP ]";
    }

    if (colour == "Green")
    {
        return "[Grn P ]";
    }

    return "[Portal]";
}