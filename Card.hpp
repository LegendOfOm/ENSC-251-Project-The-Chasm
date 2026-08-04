#ifndef CARD_HPP
#define CARD_HPP
#include "ModifierCard.hpp"
#include "Booster.hpp"
#include "Dynamite.hpp"
#include "Multiplier.hpp"
#include "Recoiler.hpp"

#include "NodeCard.hpp"
#include "BoosterNode.hpp"
#include "DiceNode.hpp"
#include "PortalNode.hpp"
#include "RecoilNode.hpp"
#include <string>

class Card
{
protected:
    std::string name;

public:
    Card(const std::string& n);

    virtual ~Card();
    
    std::string getName() const;
    // Returns a fixed-width card label
    virtual std::string output() const;
};
#endif