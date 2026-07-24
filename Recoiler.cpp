#include "Recoiler.hpp"
#include <string>

Recoiler::Recoiler(int amount): ModifierCard("Recoiler"), recoilAmount(amount){}

int Recoiler::getAmount() const
{
    return -recoilAmount;
}