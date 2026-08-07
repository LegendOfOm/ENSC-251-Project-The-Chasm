#include "Dynamite.hpp"

Dynamite::Dynamite(): ModifierCard("Dynamite"){}

int Dynamite::getModifiedAmount(int originalMovment) {
    return originalMovment;
}
