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

PortalNode* PortalNode::clone() const
{
    return new PortalNode(*this);
}
