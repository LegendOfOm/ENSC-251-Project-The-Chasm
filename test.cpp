#include <cassert>
#include <iostream>
#include <string>

#include "Bridge.hpp"
#include "Visual.hpp"
#include "card/Card.hpp"
#include "card/NodeCard.hpp"
#include "card/ModifierCard.hpp"
#include "card/nodecards/BoosterNode.hpp"
#include "card/nodecards/RecoilNode.hpp"
#include "card/nodecards/DiceNode.hpp"
#include "card/nodecards/PortalNode.hpp"
#include "card/modifiers/Booster.hpp"
#include "card/modifiers/Recoiler.hpp"
#include "card/modifiers/Multiplier.hpp"
#include "card/modifiers/Dynamite.hpp"
#include "Deck.hpp"
#include "Player.hpp"
#include "GameLogic.hpp"

// =====================================================================================
// Card
// =====================================================================================
void testCard()
{
    Card card("Boost Node");

    assert(card.getName() == "Boost Node");       // the name given to the constructor comes back
    assert(card.output() == "Boost Node");        // the label drawn on the bridge is just the name
    assert(card.Handoutput() == "Boost Node");    // and so is the label drawn in a hand

    Card empty("");

    assert(empty.getName() == "");            // an empty name is accepted
    assert(empty.output() == "");             // and prints as nothing
    assert(empty.Handoutput() == "");

    Card copy(card);

    assert(copy.getName() == "Boost Node");   // copying a card copies the name

    Card* cloned = card.clone();

    assert(cloned != &card);                     // clone makes a new object
    assert(cloned->getName() == "Boost Node");   // with the same name

    delete cloned;

    // A card deleted through a Card pointer must run the right destructor. Card has a virtual destructor so this is safe.
    Card* pointer = new BoostNode(2);

    assert(pointer->getName() == "Boost Node");   // a BoostNode is a Card and keeps its name

    delete pointer;
}

// =====================================================================================
// NodeCard
// =====================================================================================
void testNodeCard()
{
    NodeCard node;

    assert(node.getName() == "Normal Node");     // the default name when none is given
    assert(node.getMovementAmount() == 0);       // a plain node does not move the player
    assert(node.output() == "Norm  ");           // the label drawn on the bridge
    assert(node.Handoutput() == "Node Card");    // the longer label drawn in a hand

    NodeCard named("Portal Node");

    assert(named.getName() == "Portal Node");  // a custom name is stored
    assert(named.output() == "Norm  ");        // output is the same no matter the name

    NodeCard* copy = node.clone();

    assert(copy != &node);                     // clone makes a new object
    assert(copy->getName() == "Normal Node");  // with the same name
    assert(copy->getMovementAmount() == 0);    // and the same movement

    delete copy;

    // output() is the fixed-width label the bridge draws; Handoutput() is the longer one a hand draws. 
    // A colour code is 5 characters ("\033[32m") plus a 4 character reset ("\033[0m"), and neither shows on screen, so only the rest counts as width.
    DiceNode dice;
    BoostNode boost(3);
    PortalNode portal("Blue");
    PortalNode noColour("Green");

    RecoilNode recoilLabel(1);

    assert(node.output().size() == 6);                  // "Norm  "
    assert(dice.output().size() == 6);                  // "Dice  "
    assert(noColour.output().size() == 6);              // "Portal"   no colour code on this one
    assert(portal.output().size() == 6 + 9);            // "Portal"
    assert(boost.output().size() == 6 + 9);             // "BNod+ "
    assert(recoilLabel.output().size() == 6 + 9);       // "RNod- "

    // Every bridge label is 6 visible characters, so the bridge lines up. 
    // Visual prints output(), a space, then the movement number padded to 3, giving a fixed width box.
    // This holds for an unrecognised portal colour too, so adding a new colour cannot knock a row out of line.
    assert(boost.output().size() - 9 == node.output().size());          // 6 and 6
    assert(recoilLabel.output().size() - 9 == node.output().size());
    assert(portal.output().size() - 9 == node.output().size());
    assert(dice.output().size() == node.output().size());
    assert(noColour.output().size() == node.output().size());
}

// =====================================================================================
// ModifierCard
// =====================================================================================
void testModifierCard()
{
    ModifierCard modifier;

    assert(modifier.getName() == "Modifier");                 // default name
    assert(modifier.output() == "Modifier");                  // modifiers never go on the bridge, so
                                                              // output() is just the inherited name
    assert(modifier.Handoutput() == "Modifier Card");         // the label drawn in a hand

    ModifierCard named("Booster");

    assert(named.getName() == "Booster");                     // a custom name is stored
    assert(named.Handoutput() == "Modifier Card");            // but the hand label ignores it

    // The base modifier changes nothing. Each subclass overrides this to do its own maths.
    int unchanged = modifier.getModifiedAmount(5);

    assert(unchanged == 5);                      // a plain modifier leaves the movement alone

    int negative = modifier.getModifiedAmount(-3);

    assert(negative == -3);                      // including a backwards movement

    ModifierCard* copy = modifier.clone();

    assert(copy != &modifier);                   // clone makes a new object
    assert(copy->getName() == "Modifier");       // with the same name

    delete copy;
}

// =====================================================================================
// BoostNode  (BoosterNode.cpp)
// =====================================================================================
void testBoostNode()
{
    BoostNode boost(3);

    assert(boost.getName() == "Boost Node");                    // the name of every boost node
    assert(boost.getMovementAmount() == 3);                     // forward movement is positive

    BoostNode twelve(12);

    assert(twelve.getMovementAmount() == 12);                    // two digit amounts are kept

    BoostNode zero(0);

    assert(zero.getMovementAmount() == 0);                       // a boost of 0 moves nobody

    BoostNode negative(-2);

    assert(negative.getMovementAmount() == -2);                  // negative is stored as given

    BoostNode* copy = boost.clone();

    assert(copy != &boost);                                  // clone makes a new object
    assert(copy->getMovementAmount() == 3);                  // and keeps the boost amount

    delete copy;
}

// =====================================================================================
// RecoilNode
// =====================================================================================
void testRecoilNode()
{
    RecoilNode recoil(2);

    assert(recoil.getName() == "Recoil Node");               // the name of every recoil node

    assert(recoil.getMovementAmount() == -2);                   // backward movement is negative

    RecoilNode zero(0);

    assert(zero.getMovementAmount() == 0);                      // a recoil of 0 moves nobody

    RecoilNode negative(-3);

    RecoilNode* copy = recoil.clone();

    assert(copy != &recoil);                                 // clone makes a new object
    assert(copy->getMovementAmount() == -2);                 // and keeps the recoil amount

    delete copy;

    BoostNode boost(2);

    assert(boost.getMovementAmount() + recoil.getMovementAmount() == 0);   // +2 and -2 cancel
}

// =====================================================================================
// DiceNode
// =====================================================================================
void testDiceNode()
{
    DiceNode dice;

    assert(dice.getName() == "Dice Node");        // the name of every dice node
    assert(dice.output() == "Dice  ");            // the label drawn on the bridge
    assert(dice.Handoutput() == "Dice   ");       // the label drawn in a hand, one space longer

    // Roll many times and check every result is one the card is allowed to give.
    for (int i = 0; i < 100; i++)
    {
        int roll = dice.rollDice();

        // the roll is one of the six faces
        assert(roll == 4 || roll == 2 || roll == 1 || roll == -1 || roll == -2 || roll == -4);
        assert(roll <= 4);                    // never rolls higher than the best face
        assert(roll >= -4);                   // never rolls lower than the worst face
    }

    assert(dice.getMovementAmount() == 0);    // what it does now: always 0, never the roll

    DiceNode* copy = dice.clone();

    assert(copy != &dice);                    // clone makes a new object
    assert(copy->getName() == "Dice Node");   // with the same name

    delete copy;
}

// =====================================================================================
// PortalNode
// =====================================================================================
void testPortalNode()
{
    PortalNode blue("Blue");

    assert(blue.getName() == "Portal Node");           // the name of every portal node
    assert(blue.getColour() == "Blue");                // the colour given to the constructor
    assert(blue.getConnectedPortal() == nullptr);      // a new portal is not linked yet

    PortalNode purple("Purple");

    blue.setConnectedPortal(&purple);
    purple.setConnectedPortal(&blue);

    assert(blue.getConnectedPortal() == &purple);      // blue now points at purple
    assert(purple.getConnectedPortal() == &blue);      // and purple points back at blue
    assert(blue.getConnectedPortal()->getConnectedPortal() == &blue);   // going through twice returns

    blue.setConnectedPortal(nullptr);

    assert(blue.getConnectedPortal() == nullptr);      // a portal can be unlinked again

    blue.setConnectedPortal(&purple);

    assert(PortalNode("Yellow").output() == "\033[33mPortal\033[0m");        // yellow portal
    assert(PortalNode("Blue").output() == "\033[34mPortal\033[0m");          // blue portal
    assert(PortalNode("Purple").output() == "\033[35mPortal\033[0m");        // purple portal
    assert(PortalNode("Orange").output() == "\033[38;5;208mPortal\033[0m");  // orange portal
    assert(PortalNode("Green").output() == "Portal");                        // unknown colour, no colour code
    assert(PortalNode("blue").output() == "Portal");                         // the check is case sensitive

    assert(PortalNode("Blue").Handoutput() == PortalNode("Blue").output());   // both labels match

    assert(PortalNode("Green").Handoutput() == "Portal ");

    PortalNode* copy = blue.clone();

    assert(copy != &blue);                             // clone makes a new object
    assert(copy->getColour() == "Blue");               // with the same colour

    assert(copy->getConnectedPortal() == &purple);     // what it does now: points at the old partner

    delete copy;
}

// =====================================================================================
// Booster
// =====================================================================================
void testBooster()
{
    Booster booster(2);

    assert(booster.getName() == "Booster");                  // the name of every booster
    assert(booster.getBoostAmount() == 2);                   // the amount comes back positive
    assert(booster.output() == "Booster");                   // never drawn on the bridge, so just the name
    assert(booster.Handoutput() == "Booster +2");            // the label drawn in a hand

    int forward = booster.getModifiedAmount(3);

    assert(forward == 5);                       // 3 + 2 = 5

    int fromZero = booster.getModifiedAmount(0);

    assert(fromZero == 2);                      // a plain node becomes a +2 node

    int fromBackwards = booster.getModifiedAmount(-5);

    assert(fromBackwards == -3);                // a recoil node is softened, not flipped

    Booster zero(0);

    assert(zero.getBoostAmount() == 0);         // a booster of 0 adds nothing

    int nothingAdded = zero.getModifiedAmount(4);

    assert(nothingAdded == 4);                  // so the node is left alone

    Booster* copy = booster.clone();

    assert(copy != &booster);                   // clone makes a new object
    assert(copy->getBoostAmount() == 2);        // and keeps the amount

    delete copy;
}

// =====================================================================================
// Recoiler
// =====================================================================================
void testRecoiler()
{
    Recoiler recoiler(2);

    assert(recoiler.getName() == "Recoiler");                 // the name of every recoiler
    assert(recoiler.output() == "Recoiler");                  // never drawn on the bridge, so just the name
    assert(recoiler.Handoutput() == "Recoiler -2");           // the label drawn in a hand

    assert(recoiler.getRecoilAmount() == -2);    // 2 goes in, -2 comes out

    int slowed = recoiler.getModifiedAmount(3);

    assert(slowed == 1);                         // 3 - 2 = 1

    int fromZero = recoiler.getModifiedAmount(0);

    assert(fromZero == -2);                      // a plain node becomes a -2 node

    int reversed = recoiler.getModifiedAmount(1);

    assert(reversed == -1);                      // a small boost is turned backwards

    Recoiler zero(0);

    assert(zero.getRecoilAmount() == 0);         // a recoiler of 0 takes nothing away

    int nothingTaken = zero.getModifiedAmount(4);

    assert(nothingTaken == 4);                   // so the node is left alone

    Recoiler* copy = recoiler.clone();

    assert(copy != &recoiler);                   // clone makes a new object
    assert(copy->getRecoilAmount() == -2);       // and keeps the amount

    delete copy;

    Booster booster(2);
    int both = recoiler.getModifiedAmount(booster.getModifiedAmount(3));

    assert(both == 3);                           // 3 + 2 - 2 = 3
}

// =====================================================================================
// Multiplier  (Mutiplier.cpp)
// =====================================================================================
void testMultiplier()
{
    Multiplier doubler(2);

    assert(doubler.getName() == "Multiplier");            // the name of every multiplier
    assert(doubler.getMultiplierAmount() == 2);           // the amount comes back as given
    assert(doubler.output() == "Multiplier");             // never drawn on the bridge, so just the name
    assert(doubler.Handoutput() == "Multiplier x2");      // the label drawn in a hand

    int doubled = doubler.getModifiedAmount(3);

    assert(doubled == 6);                               // 3 * 2 = 6

    int doubledBackwards = doubler.getModifiedAmount(-3);

    assert(doubledBackwards == -6);                     // a recoil is doubled backwards

    int doubledNothing = doubler.getModifiedAmount(0);

    assert(doubledNothing == 0);                        // a plain node stays a plain node

    Multiplier identity(1);
    int unchanged = identity.getModifiedAmount(7);

    assert(unchanged == 7);                             // x1 changes nothing

    Multiplier zero(0);
    int cancelled = zero.getModifiedAmount(7);

    assert(cancelled == 0);                             // x0 cancels the node completely

    Multiplier negative(-1);
    int flipped = negative.getModifiedAmount(7);

    assert(flipped == -7);                              // a negative multiplier reverses the node

    Multiplier* copy = doubler.clone();

    assert(copy != &doubler);                           // clone makes a new object
    assert(copy->getMultiplierAmount() == 2);           // and keeps the amount

    delete copy;

    Booster earlier(2);
    Multiplier tripler(3);
    int runningTotal = earlier.getModifiedAmount(0);

    assert(runningTotal == 2);                          // plain node 0, booster makes it 2

    runningTotal = tripler.getModifiedAmount(runningTotal);

    assert(runningTotal == 6);                          // then the multiplier makes it 6

    int otherOrder = earlier.getModifiedAmount(tripler.getModifiedAmount(0));

    assert(otherOrder == 2);                            // 0 * 3 = 0, then + 2 = 2
    assert(otherOrder != runningTotal);                 // so order really does matter
}

// =====================================================================================
// Dynamite
// =====================================================================================
void testDynamite()
{
    Dynamite dynamite;

    assert(dynamite.getName() == "Dynamite");         // the name of every dynamite
    assert(dynamite.output() == "Dynamite");          // never drawn on the bridge, so just the name
    assert(dynamite.Handoutput() == "Dynamite");      // the label drawn in a hand

    int unchanged = dynamite.getModifiedAmount(5);

    assert(unchanged == 5);                         // the movement is not touched

    ModifierCard* copy = dynamite.clone();

    assert(copy->getName() == "Dynamite");          // the copy still says "Dynamite"

    assert(dynamic_cast<Dynamite*>(copy) == nullptr);     // the copy is a plain ModifierCard

    delete copy;
}

// =====================================================================================
// Deck
// =====================================================================================
void testDeck()
{
    Deck deck;

    assert(deck.getDeckSize() == 0);               // a brand new deck has no cards
    assert(deck.isEmpty());                        // and reports itself as empty
    assert(deck.drawCard() == nullptr);            // drawing from an empty deck gives nothing
    assert(deck.drawCardAtIndex(0) == nullptr);    // same for drawing at an index
    assert(deck.getCard(0) == nullptr);            // and there is no card to look at
    assert(deck.outputDeck() == "");               // an empty deck prints nothing

    deck.shuffleDeck();

    assert(deck.getDeckSize() == 0);               // shuffling an empty deck is safe and changes nothing

    deck.generateDeckForState(12, 1, 12);

    assert(deck.getDeckSize() == 50);          // every tier builds exactly 50 cards
    assert(!deck.isEmpty());                   // so the deck is no longer empty
    assert(deck.getCard(0) != nullptr);        // and they are all real cards
    assert(deck.getCard(49) != nullptr);
    assert(deck.getCard(50) == nullptr);       // nothing past the end
    assert(deck.getCard(-1) == nullptr);       // a negative index gives nothing

    for (int i = 0; i < deck.getDeckSize(); i++)
    {
        Card* card = deck.getCard(i);
        bool isNode = (dynamic_cast<NodeCard*>(card) != nullptr);
        bool isModifier = (dynamic_cast<ModifierCard*>(card) != nullptr);

        assert(card != nullptr);               // no empty slots
        assert(isNode || isModifier);          // and no plain Card objects
    }

    // ---- drawCard ----
    Card* expectedTop = deck.getCard(49);      // remember what is on top before drawing
    Card* top = deck.drawCard();

    assert(top == expectedTop);                 // drawCard takes from the top, the last index
    assert(deck.getDeckSize() == 49);           // the deck shrank by one
    assert(deck.getCard(49) == nullptr);        // and that slot is now empty

    delete top;                                 // the deck no longer owns this card, we do

    // ---- drawCardAtIndex ----
    Card* expectedFirst = deck.getCard(0);      // remember the first two before drawing
    Card* expectedSecond = deck.getCard(1);
    Card* first = deck.drawCardAtIndex(0);

    assert(first == expectedFirst);                 // takes the card at that index
    assert(deck.getDeckSize() == 48);               // the deck shrank by one
    assert(deck.getCard(0) == expectedSecond);      // the cards behind it shift down

    delete first;

    assert(deck.drawCardAtIndex(-1) == nullptr);    // a negative index draws nothing
    assert(deck.drawCardAtIndex(48) == nullptr);    // and so does one past the last card
    assert(deck.getDeckSize() == 48);               // a refused draw does not change the deck

    Card* before = deck.getCard(9);
    Card* target = deck.getCard(10);
    Card* after = deck.getCard(11);
    Card* middle = deck.drawCardAtIndex(10);

    assert(middle == target);                   // the card asked for is the card returned
    assert(deck.getDeckSize() == 47);           // the deck shrank by one
    assert(deck.getCard(9) == before);          // the card in front did not move
    assert(deck.getCard(10) == after);          // and the one behind shifted down into the gap

    delete middle;

    // ---- outputDeck ----
    Card* lastTwoFirst = deck.getCard(0);
    Card* lastTwoSecond = deck.getCard(1);

    while (deck.getDeckSize() > 2)
    {
        delete deck.drawCard();                 // drawing from the top leaves the first two
    }

    assert(deck.getCard(0) == lastTwoFirst);    // the two survivors are the ones expected
    assert(deck.getCard(1) == lastTwoSecond);
    assert(deck.outputDeck() == lastTwoFirst->output() + "\n" + lastTwoSecond->output() + "\n");

    // ---- emptying the deck ----
    while (!deck.isEmpty())
    {
        delete deck.drawCard();
    }

    assert(deck.getDeckSize() == 0);           // drawing every card empties the deck exactly
    assert(deck.isEmpty());                    // and it reports itself as empty
    assert(deck.drawCard() == nullptr);        // and it stays empty
    assert(deck.outputDeck() == "");           // and prints nothing

    // ---- shuffleDeck ----
    deck.generateDeckForState(12, 1, 12);

    Card* beforeShuffle[50];
    for (int i = 0; i < 50; i++)
    {
        beforeShuffle[i] = deck.getCard(i);
    }

    deck.shuffleDeck();

    assert(deck.getDeckSize() == 50);          // shuffling does not gain or lose cards
    assert(deck.getCard(50) == nullptr);       // and the deck did not grow

    for (int i = 0; i < 50; i++)
    {
        int found = 0;

        for (int j = 0; j < 50; j++)
        {
            if (deck.getCard(j) == beforeShuffle[i]) found++;
        }

        assert(found == 1);                    // every card is still there, exactly once
    }


    deck.generateDeckForState(30, 15, 20);

    assert(deck.getDeckSize() == 50);          // regenerating clears the old deck, it does not add to it
    assert(deck.getCard(49) != nullptr);       // and every slot it claims really holds a card
    assert(deck.getCard(50) == nullptr);

    deck.generateDeckForState(12, 1, 12);

    assert(deck.getDeckSize() == 50);          // still a full deck
    assert(deck.getCard(49) != nullptr);       // built from a real tier, not a half filled one
}

// =====================================================================================
// Player
// =====================================================================================
void testPlayer()
{
    Node castle;
    Node middle;

    Player player(1, &castle);

    assert(player.getPlayerId() == 1);              // the id given to the constructor
    assert(player.getCurrentNode() == &castle);     // a player starts on their own castle
    assert(player.getCastleNode() == &castle);      // and remembers which castle is theirs
    assert(player.getHandSize() == 0);              // with an empty hand
    assert(player.isHandFull() == false);           // that is not full

    // ---- moving ----
    player.setCurrentNode(&middle);

    assert(player.getCurrentNode() == &middle);     // the player moved
    assert(player.getCastleNode() == &castle);      // the castle does not move with them

    // ---- filling the hand ----
    Card* cardA = new Card("A");
    Card* cardB = new Card("B");
    Card* cardC = new Card("C");
    Card* cardD = new Card("D");
    Card* cardE = new Card("E");
    Card* cardF = new Card("F");

    bool addedA = player.addCardToHand(cardA);

    assert(addedA == true);                         // adding to an empty hand works
    assert(player.getHandSize() == 1);              // the hand grew by one
    assert(player.getCardInHand(0) == cardA);       // and holds the card we added

    player.addCardToHand(cardB);
    player.addCardToHand(cardC);
    player.addCardToHand(cardD);

    bool addedE = player.addCardToHand(cardE);

    assert(addedE == true);                         // the fifth card still fits
    assert(player.getHandSize() == 5);              // the hand is now at MAX_HAND_SIZE
    assert(player.isHandFull() == true);            // so it reports itself as full

    bool addedF = player.addCardToHand(cardF);

    assert(addedF == false);                        // a sixth card is refused
    assert(player.getHandSize() == 5);              // and the hand did not change

    bool addedNull = player.addCardToHand(nullptr);

    assert(addedNull == false);                     // a null card is refused
    assert(player.getHandSize() == 5);              // and does not take up a slot

    assert(player.getCardInHand(-1) == nullptr);    // a negative index gives nothing
    assert(player.getCardInHand(5) == nullptr);     // and so does one past the last card

    // ---- removing from the hand ----
    bool removedB = player.removeCardFromHand(cardB);

    assert(removedB == true);                       // a card that is held can be removed
    assert(player.getHandSize() == 4);              // the hand shrank by one
    assert(player.getCardInHand(0) == cardA);       // the card before it did not move
    assert(player.getCardInHand(1) == cardC);       // the cards after it shifted down
    assert(player.getCardInHand(4) == nullptr);     // and the last slot is empty now

    bool removedAgain = player.removeCardFromHand(cardB);

    assert(removedAgain == false);                  // removing a card twice fails

    bool removedUnknown = player.removeCardFromHand(cardF);

    assert(removedUnknown == false);                // a card never in the hand cannot be removed
    assert(player.isHandFull() == false);           // there is room again after a removal

    // ---- activated nodes ----
    assert(player.hasActivated(&middle) == false);  // nothing has been activated yet

    player.markActivated(&middle);

    assert(player.hasActivated(&middle) == true);   // the node is remembered
    assert(player.hasActivated(&castle) == false);  // other nodes are not affected

    player.markActivated(&middle);

    assert(player.hasActivated(&middle) == true);   // marking the same node twice is harmless

    player.resetActivatedCardsForNewTurn();

    assert(player.hasActivated(&middle) == false);  // a new turn clears the list

    // ---- winning ----
    delete cardA;
    delete cardB;
    delete cardC;
    delete cardD;
    delete cardE;
    delete cardF;

    assert(castle.nodeCard == nullptr);    // a Node made by hand holds no card
    assert(middle.nodeCard == nullptr);
    assert(castle.movementAmount == 0);    // and moves nobody
}

// =====================================================================================
// Bridge
// =====================================================================================
void testBridge()
{
    Bridge bridge;

    Node* castle1 = bridge.getPlayer1Castle();
    Node* castle2 = bridge.getPlayer2Castle();

    assert(castle1 != nullptr);                   // the bridge has a castle for player 1
    assert(castle2 != nullptr);                   // and one for player 2
    assert(castle1 != castle2);                   // they are two different nodes
    assert(castle1->isPlayer1Castle == true);     // the left end is flagged as player 1's
    assert(castle2->isPlayer2Castle == true);     // the right end is flagged as player 2's
    assert(castle1->left == nullptr);             // nothing to the left of player 1's castle
    assert(castle2->right == nullptr);            // nothing to the right of player 2's castle

    assert(bridge.castleNodeCheck(castle1) == 1);         // 1 means player 1's castle
    assert(bridge.castleNodeCheck(castle2) == 2);         // 2 means player 2's castle
    assert(bridge.castleNodeCheck(castle1->right) == 0);  // 0 means an ordinary node

    int count = 0;
    for (Node* walk = castle1; walk != nullptr; walk = walk->right)
    {
        count++;
    }

    assert(count == 12);                          // 12 nodes to start: 2 castles and 10 between

    int backwards = 0;
    for (Node* walk = castle2; walk != nullptr; walk = walk->left)
    {
        backwards++;
    }

    assert(backwards == 12);                      // walking back gives the same count, so the links match

    assert(castle1->right->left == castle1);      // going right then left comes back

    assert(castle1->nodeCard != nullptr);                     // even the castles hold a card
    assert(castle1->nodeCard->getName() == "Normal Node");    // a plain one
    assert(castle1->movementAmount == 0);                     // that moves nobody
    assert(castle1->beginningOfStrand == nullptr);            // and has no modifiers

    int castleMovement = bridge.getMovementOnBridge(castle1);

    assert(castleMovement == 0);                              // and the getter agrees

    // ---- isValidNode ----
    assert(bridge.isValidNode(1) == false);       // player 1's castle cannot be edited
    assert(bridge.isValidNode(2) == true);        // the first node after it can
    assert(bridge.isValidNode(11) == true);       // and so can the last node before the other castle
    assert(bridge.isValidNode(12) == false);      // player 2's castle cannot be edited
    assert(bridge.isValidNode(0) == false);       // before the bridge
    assert(bridge.isValidNode(13) == false);      // past the bridge

    // ---- isValidNodePlacement ----
    assert(bridge.isValidNodePlacement(1, 2) == true);     // a card can go right next to player 1's castle
    assert(bridge.isValidNodePlacement(11, 12) == true);   // and right next to player 2's castle
    assert(bridge.isValidNodePlacement(0, 1) == false);    // but not past player 1's castle
    assert(bridge.isValidNodePlacement(12, 13) == false);  // and not past player 2's castle
    assert(bridge.isValidNodePlacement(2, 4) == false);    // the two nodes must be next to each other
    assert(bridge.isValidNodePlacement(4, 3) == false);    // and given left first, then right
    assert(bridge.isValidNodePlacement(3, 3) == false);    // the same node twice is not a gap

    // ---- insertCard ----
    NodeCard* rejected = new BoostNode(1);
    bool badInsert = bridge.insertCard(12, 13, rejected);

    assert(badInsert == false);                   // a card cannot be placed past a castle

    delete rejected;                              // a rejected card is still ours to delete

    NodeCard* boost = new BoostNode(3);
    bool goodInsert = bridge.insertCard(1, 2, boost);

    assert(goodInsert == true);                   // a card between two real neighbours is accepted

    Node* inserted = castle1->right;

    assert(inserted->nodeCard == boost);          // the new node holds the card we passed in
    assert(inserted->nodeCard->getMovementAmount() == 3);    // and it is the +3 boost
    assert(inserted->movementAmount == 3);        // the node remembers how far it moves a player
    assert(inserted->left == castle1);            // linked to the node on its left
    assert(inserted->right->left == inserted);    // and the node on its right links back
    assert(inserted->isPlayer1Castle == false);   // a new node is never a castle
    assert(inserted->isPlayer2Castle == false);

    count = 0;
    for (Node* walk = castle1; walk != nullptr; walk = walk->right)
    {
        count++;
    }

    assert(count == 13);                          // the bridge grew by one node

    int insertedMovement = bridge.getMovementOnBridge(inserted);

    assert(insertedMovement == 3);                // the getter reads back the stored amount

    bool dicePlaced = bridge.insertCard(2, 3, new DiceNode());

    assert(dicePlaced == true);
    assert(bridge.getMovementOnBridge(inserted->right) == 0);   // the dice node stores nothing

    bool portalPlaced = bridge.insertCard(3, 4, new PortalNode("Blue"));

    assert(portalPlaced == true);
    assert(bridge.getMovementOnBridge(inserted->right->right) == 0);   // and neither does the portal

    bool portalRemoved = bridge.removeNode(4);   // take the portal back off, it is node 4
    bool diceRemoved = bridge.removeNode(3);     // then the dice, leaving the boost node alone

    assert(portalRemoved == true);
    assert(diceRemoved == true);
    assert(castle1->right == inserted);          // the boost node is back next to the castle

    int countBeforeNull = 0;
    for (Node* walk = castle1; walk != nullptr; walk = walk->right)
    {
        countBeforeNull++;
    }

    bool nullInsert = bridge.insertCard(1, 2, nullptr);

    assert(nullInsert == false);                 // a null card cannot be placed

    int countAfterNull = 0;
    for (Node* walk = castle1; walk != nullptr; walk = walk->right)
    {
        countAfterNull++;
    }

    assert(countAfterNull == countBeforeNull);   // and no empty node was left behind
    assert(castle1->right == inserted);          // the bridge is untouched

    // ---- attachModifierCard ----
    bool attached = bridge.attachModifierCard(2, new Booster(1));

    assert(attached == true);                                                    // a modifier can be attached to a real node
    assert(inserted->beginningOfStrand != nullptr);                              // the node now has a strand
    assert(inserted->beginningOfStrand->modifierCard->getName() == "Booster");   // holding our booster
    assert(inserted->beginningOfStrand->next == nullptr);                        // and nothing after it

    assert(inserted->movementAmount == 4);        // 3 + 1, the booster was applied

    bridge.attachModifierCard(2, new Multiplier(2));

    assert(inserted->beginningOfStrand->modifierCard->getName() == "Booster");            // the first modifier stays first
    assert(inserted->beginningOfStrand->next->modifierCard->getName() == "Multiplier");   // the new one goes on the end

    assert(inserted->movementAmount == 8);              // (3 + 1) * 2 = 8
    assert(bridge.getMovementOnBridge(inserted) == 8);  // and the getter agrees

    Bridge orderCheck;

    orderCheck.insertCard(1, 2, new BoostNode(3));
    orderCheck.attachModifierCard(2, new Multiplier(2));    // multiplier lands first this time
    orderCheck.attachModifierCard(2, new Booster(1));       // then the booster

    Node* orderedNode = orderCheck.getPlayer1Castle()->right;

    assert(orderedNode->beginningOfStrand->modifierCard->getName() == "Multiplier");   // strand order is kept
    assert(orderedNode->beginningOfStrand->next->modifierCard->getName() == "Booster");
    assert(orderCheck.getMovementOnBridge(orderedNode) == 7);   // 3 * 2 = 6, then 6 + 1 = 7

    assert(orderCheck.getMovementOnBridge(orderedNode) != inserted->movementAmount);

    Bridge worked;

    worked.insertCard(1, 2, new NodeCard());          // a plain node, movement 0
    worked.attachModifierCard(2, new Booster(2));     // turn 1
    worked.attachModifierCard(2, new Multiplier(3));  // turn 2
    worked.attachModifierCard(2, new Multiplier(2));  // turn 3, the multiplier is placed first
    worked.attachModifierCard(2, new Booster(1));     // turn 3, then the booster

    Node* workedNode = worked.getPlayer1Castle()->right;

    assert(workedNode->beginningOfStrand->modifierCard->getName() == "Booster");                    // strand order
    assert(workedNode->beginningOfStrand->next->modifierCard->getName() == "Multiplier");
    assert(workedNode->beginningOfStrand->next->next->modifierCard->getName() == "Multiplier");
    assert(workedNode->beginningOfStrand->next->next->next->modifierCard->getName() == "Booster");

    assert(worked.getMovementOnBridge(workedNode) == 13);     // 0 +2 = 2, x3 = 6, x2 = 12, +1 = 13

    ModifierCard* onCastle = new Booster(1);
    bool castleAttach = bridge.attachModifierCard(1, onCastle);

    assert(castleAttach == false);                     // castles cannot be modified

    delete onCastle;                                   // a rejected modifier is still ours to delete

    ModifierCard* offBridge = new Booster(1);
    bool offAttach = bridge.attachModifierCard(999, offBridge);

    assert(offAttach == false);                        // a node that does not exist is refused

    delete offBridge;

    // ---- removeNode ----
    Node* third = inserted->right;

    bool removed = bridge.removeNode(2);

    assert(removed == true);                     // a normal node can be destroyed
    assert(castle1->right == third);             // the nodes on either side are joined together
    assert(third->left == castle1);              // in both directions

    count = 0;
    for (Node* walk = castle1; walk != nullptr; walk = walk->right)
    {
        count++;
    }

    assert(count == 12);                         // the bridge shrank back by one node

    bool removedCastle = bridge.removeNode(1);

    assert(removedCastle == false);              // player 1's castle cannot be destroyed

    bool removedOtherCastle = bridge.removeNode(12);

    assert(removedOtherCastle == false);         // player 2's castle cannot be destroyed

    bool removedNothing = bridge.removeNode(0);

    assert(removedNothing == false);             // a node that does not exist cannot be destroyed

    // ---- copy constructor ----
    Bridge copy(bridge);

    assert(copy.getPlayer1Castle() != bridge.getPlayer1Castle());   // the copy has its own nodes
    assert(copy.getPlayer2Castle() != bridge.getPlayer2Castle());   // at both ends
    assert(copy.getPlayer1Castle()->isPlayer1Castle == true);       // and the castles are still flagged
    assert(copy.getPlayer2Castle()->isPlayer2Castle == true);

    int copyCount = 0;
    for (Node* walk = copy.getPlayer1Castle(); walk != nullptr; walk = walk->right)
    {
        copyCount++;
    }

    assert(copyCount == 12);                     // the copy is the same length as the original

    assert(copy.getPlayer1Castle()->nodeCard != nullptr);                         // the castles get a card
    assert(copy.getPlayer2Castle()->nodeCard != nullptr);
    assert(copy.getPlayer1Castle()->nodeCard != castle1->nodeCard);               // a new one, not shared
    assert(copy.getPlayer1Castle()->nodeCard->getName() == "Normal Node");        // and a plain one

    assert(copy.getPlayer1Castle()->right->nodeCard != nullptr);                  // the middle nodes get a card
    assert(copy.getPlayer1Castle()->right->nodeCard != castle1->right->nodeCard); // and it is a new one

    // ---- assignment ----
    Bridge target;
    target = bridge;

    assert(target.getPlayer1Castle() != bridge.getPlayer1Castle());   // assignment also makes its own nodes

    target = target;

    assert(target.getPlayer1Castle() != nullptr);   // assigning a bridge to itself does not destroy it

    // ---- shrinking down to nothing ----
    Bridge small;

    for (int i = 0; i < 10; i++)
    {
        bool shrink = small.removeNode(2);

        assert(shrink == true);                  // all 10 middle nodes can be destroyed
    }

    bool lastRemove = small.removeNode(2);

    assert(lastRemove == false);                                          // once only the castles are left nothing can be removed
    assert(small.getPlayer1Castle()->right == small.getPlayer2Castle());   // the two castles are joined directly
    assert(small.getPlayer2Castle()->left == small.getPlayer1Castle());    // in both directions

    Bridge smallCopy(small);

    int smallCopyCount = 0;
    for (Node* walk = smallCopy.getPlayer1Castle(); walk != nullptr; walk = walk->right)
    {
        smallCopyCount++;
    }

    assert(smallCopyCount == 2);                                                  // 2 nodes in, 2 nodes out
    assert(smallCopy.getPlayer1Castle()->right == smallCopy.getPlayer2Castle());  // joined directly
    assert(smallCopy.getPlayer2Castle()->left == smallCopy.getPlayer1Castle());
}

// =====================================================================================
// Visual
//
// Every function in Visual prints to the screen and returns nothing, so there is no
// return value to assert on. This function builds a small board and prints it, and the
// output has to be checked by eye against what is written in the comments.
// =====================================================================================
void testVisual()
{
    Bridge bridge;

    bridge.insertCard(1, 2, new BoostNode(2));
    bridge.insertCard(2, 3, new RecoilNode(1));
    bridge.insertCard(3, 4, new PortalNode("Blue"));
    bridge.insertCard(4, 5, new DiceNode());
    bridge.attachModifierCard(2, new Booster(1));
    bridge.attachModifierCard(2, new Multiplier(2));

    Player player1(1, bridge.getPlayer1Castle());
    Player player2(2, bridge.getPlayer2Castle());

    player1.setCurrentNode(bridge.getPlayer1Castle()->right);

    Card* handCard1 = new BoostNode(3);
    Card* handCard2 = new Dynamite();

    player1.addCardToHand(handCard1);
    player1.addCardToHand(handCard2);

    Visual visual;

    std::cout << "printBridge:" << std::endl;
    visual.printBridge(bridge, player1, player2);
    std::cout << std::endl;

    std::cout << "printHands:" << std::endl;
    visual.printHands(player1, player2);

    delete handCard1;
    delete handCard2;
}

// =====================================================================================
// GameLogic
// =====================================================================================
void testGameLogic()
{
    // Building a GameLogic must set up a bridge, a deck and two players on their castles
    // without crashing. There are no get functions on GameLogic, so this is all that can
    // be checked from outside.
    GameLogic game;
}

// =====================================================================================
// Run every test
// =====================================================================================
int main()
{
    testCard();
    std::cout << "Card passed" << std::endl;

    testNodeCard();
    std::cout << "NodeCard passed" << std::endl;

    testModifierCard();
    std::cout << "ModifierCard passed" << std::endl;

    testBoostNode();
    std::cout << "BoostNode passed" << std::endl;

    testRecoilNode();
    std::cout << "RecoilNode passed" << std::endl;

    testDiceNode();
    std::cout << "DiceNode passed" << std::endl;

    testPortalNode();
    std::cout << "PortalNode passed" << std::endl;

    testBooster();
    std::cout << "Booster passed" << std::endl;

    testRecoiler();
    std::cout << "Recoiler passed" << std::endl;

    testMultiplier();
    std::cout << "Multiplier passed" << std::endl;

    testDynamite();
    std::cout << "Dynamite passed" << std::endl;

    testDeck();
    std::cout << "Deck passed" << std::endl;

    testPlayer();
    std::cout << "Player passed" << std::endl;

    testBridge();
    std::cout << "Bridge passed" << std::endl;

    testGameLogic();
    std::cout << "GameLogic passed" << std::endl;

    std::cout << std::endl;
    std::cout << "All tests passed." << std::endl;
    std::cout << std::endl;

    testVisual();

    return 0;
}
