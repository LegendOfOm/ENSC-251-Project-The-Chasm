#include "Dynamite.hpp"

Dynamite::Dynamite(): ModifierCard("Dynamite"){}

int Dynamite::getModifiedAmount(int originalMovment) {
    return originalMovment;
}

std::string Dynamite::Handoutput() const {
    return "Dynamite";
}
