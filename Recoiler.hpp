#ifndef RECOILER_HPP
#define RECOILER_HPP

#include "ModifierCard.hpp"

class Recoiler : public ModifierCard
{
private:
    int recoilAmount;

public:
    Recoiler(int amount);
    int getRecoilAmount() const;
    virtual Recoiler* clone() const;
    virtual int getModifiedAmount(int originalMovment);

};

#endif