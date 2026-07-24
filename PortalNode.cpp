#include "PortalNode.hpp"

PortalNode::PortalNode(): NodeCard("Portal Node"), connectedPortal(nullptr){}

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
    return "[Portal]";
}