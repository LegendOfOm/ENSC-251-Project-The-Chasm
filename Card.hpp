#ifndef CARD_HPP
#define CARD_HPP

#include <string>

class Card
{
protected:
    std::string name;

public:
    Card(const std::string& n);

    virtual ~Card();

    virtual Card* clone() const;
    
    std::string getName() const;
    // Returns a fixed-width card label
    virtual std::string output() const;
};
#endif