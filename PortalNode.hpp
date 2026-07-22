#ifndef PORTALNODE_HPP
#define PORTALNODE_HPP

#include "NodeCard.hpp"

class PortalNode : public NodeCard
{
public:
    PortalNode();
    std::string output() const override;
};

#endif