#ifndef PORTALNODE_HPP
#define PORTALNODE_HPP

#include "NodeCard.hpp"

class PortalNode : public NodeCard
{
private:
    std::string colour;
    PortalNode* connectedPortal;

public:
    PortalNode(const std::string& portalColour);
    std::string getColour() const;
    void setConnectedPortal(PortalNode* portal);
    PortalNode* getConnectedPortal() const;

    std::string output() const override;
};

#endif