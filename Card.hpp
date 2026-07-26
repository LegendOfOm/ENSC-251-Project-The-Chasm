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
    // Returns a fixed-width card label
    virtual std::string output() const;
    virtual Card* clone() const = 0;
};
#endif