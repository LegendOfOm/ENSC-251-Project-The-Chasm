#ifndef RECOILNODE_HPP
#define RECOILNODE_HPP

#include "NodeCard.hpp"

class RecoilNode : public NodeCard
{
private:
    int recoilAmount;

public:
    RecoilNode(int amount);

    int getMovementAmount() const override;
    std::string output() const override;
    RecoilNode* clone() const override;
    std::string Handoutput() const override;
};

#endif