#include "Recoiler.hpp"
#include <string>

Recoiler::Recoiler(int amount): ModifierCard("Recoiler"), recoilAmount(amount){}

int Recoiler::getRecoilAmount() const
{
    return -recoilAmount;
}

Recoiler* Recoiler::clone() const
{
    return new Recoiler(*this);
}