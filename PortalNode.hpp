#ifndef PORTALNODE_HPP
#define PORTALNODE_HPP

#include "NodeCard.hpp"

class PortalNode : public NodeCard
{
private:
    PortalNode* connectedPortal;

public:
    PortalNode();

    void setConnectedPortal(PortalNode* portal);
    PortalNode* getConnectedPortal() const;

    std::string output() const override;
};

#endif