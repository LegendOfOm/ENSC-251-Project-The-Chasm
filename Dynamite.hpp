#ifndef DYNAMITE_HPP
#define DYNAMITE_HPP

#include "ModifierCard.hpp"

class Dynamite : public ModifierCard
{
public:
    Dynamite();
    std::string Handoutput() const override;
    virtual int getModifiedAmount(int originalMovment);

};

#endif