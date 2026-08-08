#ifndef CARD_HPP
#define CARD_HPP

#include <string>

class Card
{
protected:
    std::string name;
     // Stores the name of the card
public:
    Card(const std::string& n);
    // precondition: n should contain the name of the card
    // postcondition: creates a Card and initializes name to n

    virtual ~Card();
    // postcondition: destroys the Card object
    // virtual so derived class destructors are also called correctly

    virtual Card* clone() const;
    // postcondition: creates and returns a dynamically allocated copy
    // of the current Card

    std::string getName() const;
    // Returns a fixed-width card label
    virtual std::string output() const;
    // postcondition: returns a fixed-width string representation
    // of the card for displaying it on the bridge

    virtual std::string Handoutput() const;
    // postcondition: returns a string representation of the card
    // for displaying it in the player's hand
};
#endif