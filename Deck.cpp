#include "Deck.hpp"
#include "card/Card.hpp"
#include "card/ModifierCard.hpp"
#include "card/modifiers/Booster.hpp"
#include "card/modifiers/Dynamite.hpp"
#include "card/modifiers/Multiplier.hpp"
#include "card/modifiers/Recoiler.hpp"
#include "card/NodeCard.hpp"
#include "card/nodecards/BoosterNode.hpp"
#include "card/nodecards/DiceNode.hpp"
#include "card/nodecards/PortalNode.hpp"
#include "card/nodecards/RecoilNode.hpp"
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include "rng.hpp"

Deck::Deck() : deckSize(0), deckNumber(0), u_previous(0) {}

Deck::~Deck() {
    clearDeck();
}

Card* Deck::drawCard() {
    if (deckSize == 0) {
        return nullptr; // Deck is empty
    }
    Card* drawnCard = cards[deckSize - 1];
    cards[deckSize - 1] = nullptr; // Remove the card from the deck
    deckSize--;
    return drawnCard;
}

Card* Deck::drawCardAtIndex(int index) {
    if (index < 0 || index >= deckSize) {
        return nullptr; // Invalid index
    }
    Card* drawnCard = cards[index];
    for (int i = index; i < deckSize - 1; ++i) {
        cards[i] = cards[i + 1]; // Shift cards down
    }
    cards[deckSize - 1] = nullptr; // Remove the last card
    deckSize--;
    return drawnCard;
}

void Deck::clearDeck() {
    for (int i = 0; i < deckSize; ++i) {
        delete cards[i];
        cards[i] = nullptr;
    }
    deckSize = 0;
}

Card* Deck::getCard(int index) const {
    if (index < 0 || index >= deckSize) {
        return nullptr; // Invalid index
    }
    return cards[index];
}

bool Deck::addCard(Card* card) {
    if (deckSize >= MAX_DECK_SIZE) return false;
    cards[deckSize] = card;
    deckSize++;
    return true;
}

void Deck::generateDeckForState(int bridgeSize, int player1NodeIndex, int player2NodeIndex) {
    clearDeck();
    // find the least distance required for one of the players to finish
    int distanceToFinishFor1 = bridgeSize - player1NodeIndex;
    int distanceToFinishFor2 = player2NodeIndex - 1;
    int minDistanceToFinish = std::min(distanceToFinishFor1, distanceToFinishFor2);
    // define constants 
    const double maxWantedDeckGeneration = 3.0;
    const double startingBridgeLength = 12.0;
    const double startingDistanceToCross = startingBridgeLength - 1;
    const double BLOAT_SPAN = 2.0; // just a number to determine when the u_bloat saturates
    const int TIERS = 4;
    const double DIST_SPAN = 2.0;
    
    
    // how far the game has gone
    double u_time  = std::clamp(deckNumber / maxWantedDeckGeneration,                                      0.0, 1.0);
    // how big the the bridge has gotten
    double u_bloat = std::clamp((bridgeSize - startingBridgeLength) / (BLOAT_SPAN * startingBridgeLength), 0.0, 1.0);
    // how far the player ahead must travel
    double u_dist = std::clamp((minDistanceToFinish - startingDistanceToCross) / (DIST_SPAN * startingDistanceToCross), 0.0, 1.0);
    // calculate raw urgency value
    double u_raw = 0.30*u_time + 0.10*u_bloat + 0.60*u_dist;
    // compare the highest urgency. this way, the urgency always increases
    double u = std::max(u_previous, u_raw);
    // calculate the tiers
    int tier = round(u * (TIERS - 1));
    
    buildTier(tier);
    
    deckNumber++;
    u_previous = u;
    shuffleDeck();
}

void Deck::buildTier(int tier)
{
    clearDeck();

    switch (tier) {
    case 0:
        for (int i = 0; i < 2; ++i) addCard(new NodeCard());          // Node
        for (int i = 0; i < 3; ++i) addCard(new BoostNode(1));        // Boost +1
        for (int i = 0; i < 7; ++i) addCard(new BoostNode(2));        // Boost +2
        for (int i = 0; i < 4; ++i) addCard(new BoostNode(3));        // Boost +3
        for (int i = 0; i < 2; ++i) addCard(new BoostNode(4));        // Boost +4
        for (int i = 0; i < 4; ++i) addCard(new RecoilNode(1));       // Recoil -1
        for (int i = 0; i < 6; ++i) addCard(new RecoilNode(2));       // Recoil -2
        for (int i = 0; i < 3; ++i) addCard(new RecoilNode(3));       // Recoil -3
        for (int i = 0; i < 1; ++i) addCard(new DiceNode());          // Dice Node
        // one linked yellow pair
        addCard(new PortalNode("Yellow"));
        addCard(new PortalNode("Yellow"));
        for (int i = 0; i < 3; ++i) addCard(new Booster(1));          // Booster +1
        for (int i = 0; i < 2; ++i) addCard(new Booster(2));          // Booster +2
        for (int i = 0; i < 3; ++i) addCard(new Recoiler(1));         // Recoiler -1
        for (int i = 0; i < 2; ++i) addCard(new Recoiler(2));         // Recoiler -2
        for (int i = 0; i < 4; ++i) addCard(new Multiplier(2));       // Multiplier x2
        for (int i = 0; i < 2; ++i) addCard(new Multiplier(3));       // Multiplier x3
        break;

    case 1:
        for (int i = 0; i < 1; ++i) addCard(new NodeCard());          // Node
        for (int i = 0; i < 4; ++i) addCard(new BoostNode(2));        // Boost +2
        for (int i = 0; i < 7; ++i) addCard(new BoostNode(3));        // Boost +3
        for (int i = 0; i < 4; ++i) addCard(new BoostNode(4));        // Boost +4
        for (int i = 0; i < 2; ++i) addCard(new BoostNode(5));        // Boost +5
        for (int i = 0; i < 1; ++i) addCard(new BoostNode(6));        // Boost +6
        for (int i = 0; i < 4; ++i) addCard(new RecoilNode(2));       // Recoil -2
        for (int i = 0; i < 5; ++i) addCard(new RecoilNode(3));       // Recoil -3
        for (int i = 0; i < 2; ++i) addCard(new RecoilNode(4));       // Recoil -4
        for (int i = 0; i < 1; ++i) addCard(new DiceNode());          // Dice Node
        // one linked blue pair
        addCard(new PortalNode("Blue"));
        addCard(new PortalNode("Blue"));
        for (int i = 0; i < 2; ++i) addCard(new Booster(1));          // Booster +1
        for (int i = 0; i < 3; ++i) addCard(new Booster(2));          // Booster +2
        for (int i = 0; i < 3; ++i) addCard(new Recoiler(1));         // Recoiler -1
        for (int i = 0; i < 2; ++i) addCard(new Recoiler(2));         // Recoiler -2
        for (int i = 0; i < 3; ++i) addCard(new Multiplier(2));       // Multiplier x2
        for (int i = 0; i < 2; ++i) addCard(new Multiplier(3));       // Multiplier x3
        for (int i = 0; i < 2; ++i) addCard(new Dynamite());          // Dynamite
        break;

    case 2:
        for (int i = 0; i < 2; ++i) addCard(new BoostNode(2));        // Boost +2
        for (int i = 0; i < 3; ++i) addCard(new BoostNode(3));        // Boost +3
        for (int i = 0; i < 2; ++i) addCard(new BoostNode(4));        // Boost +4
        for (int i = 0; i < 1; ++i) addCard(new BoostNode(5));        // Boost +5
        for (int i = 0; i < 2; ++i) addCard(new RecoilNode(2));       // Recoil -2
        for (int i = 0; i < 1; ++i) addCard(new RecoilNode(3));       // Recoil -3
        for (int i = 0; i < 1; ++i) addCard(new DiceNode());          // Dice Node
        // one linked purple pair
        addCard(new PortalNode("Purple"));
        addCard(new PortalNode("Purple"));
        for (int i = 0; i < 4; ++i) addCard(new Booster(1));          // Booster +1
        for (int i = 0; i < 4; ++i) addCard(new Booster(2));          // Booster +2
        for (int i = 0; i < 3; ++i) addCard(new Recoiler(1));         // Recoiler -1
        for (int i = 0; i < 2; ++i) addCard(new Recoiler(2));         // Recoiler -2
        for (int i = 0; i < 3; ++i) addCard(new Multiplier(2));       // Multiplier x2
        for (int i = 0; i < 2; ++i) addCard(new Multiplier(3));       // Multiplier x3
        for (int i = 0; i < 18; ++i) addCard(new Dynamite());          // Dynamite
        break;

    case 3:
        for (int i = 0; i < 2; ++i) addCard(new BoostNode(3));        // Boost +3
        for (int i = 0; i < 2; ++i) addCard(new BoostNode(4));        // Boost +4
        for (int i = 0; i < 1; ++i) addCard(new BoostNode(5));        // Boost +5
        for (int i = 0; i < 1; ++i) addCard(new BoostNode(6));        // Boost +6
        for (int i = 0; i < 1; ++i) addCard(new RecoilNode(3));       // Recoil -3
        // one linked orange pair
        addCard(new PortalNode("Orange"));
        addCard(new PortalNode("Orange"));
        for (int i = 0; i < 3; ++i) addCard(new Booster(1));          // Booster +1
        for (int i = 0; i < 3; ++i) addCard(new Booster(2));          // Booster +2
        for (int i = 0; i < 2; ++i) addCard(new Recoiler(1));         // Recoiler -1
        for (int i = 0; i < 3; ++i) addCard(new Multiplier(2));       // Multiplier x2
        for (int i = 0; i < 2; ++i) addCard(new Multiplier(3));       // Multiplier x3
        for (int i = 0; i < 28; ++i) addCard(new Dynamite());          // Dynamite
        break;

    default:
        buildTier(0);
        break;
    }
}

void Deck::shuffleDeck() {
    std::shuffle(cards, cards + deckSize, rng::randomNumber);
}


std::string Deck::outputDeck() const {
    std::string output;
    for (int i = 0; i < deckSize; ++i) {
        output += cards[i]->output() + "\n";
    }
    return output;
}

bool Deck::isEmpty() const {
    if (deckSize == 0) {
        return true; // Deck is empty
    } else {
        return false; // Deck is not empty
    }
}

int Deck::getDeckSize() const { return deckSize; }