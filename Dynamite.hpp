#ifndef DYNAMITE_HPP
#define DYNAMITE_HPP

#include "ModifierCard.hpp"

class Dynamite : public ModifierCard
{
public:
    Dynamite();
    std::string output() const override;
};

#endif