#ifndef BOOSTNODE_HPP
#define BOOSTNODE_HPP

#include "NodeCard.hpp"

class BoostNode : public NodeCard
{
private:
    int boostAmount;

public:
    BoostNode(int amount);

    int getMovementAmount() const override;
    std::string output() const override;
};

#endif