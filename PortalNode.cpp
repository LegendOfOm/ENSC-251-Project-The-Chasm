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
        return "\033[31m[Portal]\033[0m";
    }

    if (colour == "Blue")
    {
        return "\033[34m[Portal]\033[0m";
    }

    if (colour == "Green")
    {
        return "\033[32m[Portal]\033[0m";
    }

    return "[Portal]";
}

PortalNode* PortalNode::clone() const
{
    return new PortalNode(*this);
}
