#ifndef CARD_HPP
#define CARD_HPP

#include <string>

class Card
{
protected:
    // Stores the name of the card
    std::string name;
public:
    // precondition: n should contain the name of the card
    // postcondition: creates a Card and initializes name to n
    Card(const std::string& n);

    // postcondition: destroys the Card object
    // virtual so derived class destructors are also called correctly
    virtual ~Card();

    // postcondition: creates and returns a dynamically allocated copy
    // of the current Card
    virtual Card* clone() const;

    // Returns a fixed-width card label
    std::string getName() const;

    // postcondition: returns a fixed-width string representation
    // of the card for displaying it on the bridge
    virtual std::string output() const;

    // postcondition: returns a string representation of the card
    // for displaying it in the player's hand
    virtual std::string Handoutput() const;
};
#endif