#ifndef CARD_HPP
#define CARD_HPP

#include <string>

class Card
{
protected:
    std::string name;

public:
    Card(const std::string& n);

    std::string getName() const;
};

#endif

#ifndef NODECARD_HPP
#define NODECARD_HPP

#include "Card.hpp"

class NodeCard : public Card
{
public:
    NodeCard(const std::string& cardName);
};

#endif


#ifndef BOOSTERNODE_HPP
#define BOOSTERNODE_HPP

#include "NodeCard.hpp"

class BoostNode : public NodeCard
{
private:
    int boostAmount;

public:
    BoostNode(int amount);

    int getBoostAmount() const;
};

#endif


#ifndef RECOILERNODE_HPP
#define RECOILERNODE_HPP

#include "NodeCard.hpp"

class RecoilNode : public NodeCard
{
private:
    int recoilAmount;

public:
    RecoilNode(int amount);

    int getRecoilAmount() const;
};

#endif


#ifndef PORTALNODE_HPP
#define PORTALNODE_HPP

#include "NodeCard.hpp"

class PortalNode : public NodeCard
{
public:
    PortalNode();
};

#endif


#ifndef DICENODE_HPP
#define DICENODE_HPP

#include "NodeCard.hpp"

class DiceNode : public NodeCard
{
public:
    DiceNode();
};

#endif