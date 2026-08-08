// =====================================================================================/// test.cpp -- unit tests for The Chasm
//
// One test function per class. Every assert checks one thing and has a comment saying
// what it checks. Run the test task and every assert should pass silently.
//
// Two tags are used in the comments:
//
//   ??? UNCLEAR -- our design does not say clearly what this function is supposed to do,
//                  so the assert just records what the code does right now. We should
//                  agree on the answer and then fix either the code or the assert.
//
//   !!! BROKEN  -- the function does not do what it is supposed to do. The assert that
//                  would catch it is commented out so the rest of the tests still run.
//
// NOTE: asserts are removed when NDEBUG is defined, so no function call that changes
// something is written inside an assert. The call is done first, the result is stored,
// and the assert only looks at the stored result.
//
// NOTE: Deck.cpp now uses std::clamp, so the project needs C++17 or newer. The build task
// has no -std flag and g++ defaults to C++17, so it works, but do not add -std=c++11 to
// the task or Deck.cpp will stop compiling.
// =====================================================================================

#include <cassert>
#include <iostream>
#include <string>

#include "Bridge.hpp"
#include "Visual.hpp"
#include "Card.hpp"
#include "NodeCard.hpp"
#include "ModifierCard.hpp"
#include "BoosterNode.hpp"
#include "RecoilNode.hpp"
#include "DiceNode.hpp"
#include "PortalNode.hpp"
#include "Booster.hpp"
#include "Recoiler.hpp"
#include "Multiplier.hpp"
#include "Dynamite.hpp"
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

    // A card deleted through a Card pointer must run the right destructor. Card has a
    // virtual destructor so this is safe.
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

    // output() is the fixed-width label the bridge draws; Handoutput() is the longer one a
    // hand draws. A colour code is 5 characters ("\033[32m") plus a 4 character reset
    // ("\033[0m"), and neither shows on screen, so only the rest counts as width.
    DiceNode dice;
    BoostNode boost(3);
    PortalNode portal("Blue");
    PortalNode noColour("Green");

    assert(node.output().size() == 6);            // "Norm  "        6 visible
    assert(dice.output().size() == 6);            // "Dice  "        6 visible
    assert(portal.output().size() == 6 + 9);      // "Portal"        6 visible
    assert(boost.output().size() == 5 + 9);       // "BNod+"         5 visible
    assert(noColour.output().size() == 7);        // "Portal "       7 visible, no colour code

    // !!! BROKEN: those visible widths do not match, so the bridge does not line up.
    // Visual::printNode() prints output(), then a space, then the movement number padded
    // to 3 characters, so a node is meant to be a fixed width box. A boost or recoil node
    // is one character narrower than a normal, dice or portal node, and an uncoloured
    // portal is one character wider. Every column after the first odd node is shifted.
    // Pad "BNod+" and "RNod-" to 6 characters and cut the fallback "Portal " to 6.
    assert(boost.output().size() - 9 != node.output().size());      // what it does now: 5 vs 6
    // assert(boost.output().size() - 9 == node.output().size());   // what we actually want
    assert(noColour.output().size() != node.output().size());       // what it does now: 7 vs 6

    // ??? UNCLEAR: NodeCard has a private movementAmount that is always 0, and every
    // subclass stores its own amount instead of using it. The base variable is never
    // read, so either the subclasses should set it or it should be removed.
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

    // On the bridge the label carries no number: Visual prints the node's own movement
    // amount next to it, which is the total after any modifiers. In a hand there is no
    // node yet, so the hand label carries the card's own amount.
    assert(boost.output() == "\033[32mBNod+\033[0m");            // green, no number
    assert(boost.Handoutput() == "\033[32mBNod+ 3\033[0m");      // green, with the number

    BoostNode twelve(12);

    assert(twelve.getMovementAmount() == 12);                    // two digit amounts are kept
    assert(twelve.output() == "\033[32mBNod+\033[0m");           // the bridge label never changes
    assert(twelve.Handoutput() == "\033[32mBNod+ 12\033[0m");    // only the hand label grows

    BoostNode zero(0);

    assert(zero.getMovementAmount() == 0);                       // a boost of 0 moves nobody

    // ??? UNCLEAR: the constructor does not reject a negative boost, so a "boost" can send
    // the player backwards and the hand label reads "BNod+ -2". Should BoostNode refuse a
    // negative amount?
    BoostNode negative(-2);

    assert(negative.getMovementAmount() == -2);                  // negative is stored as given
    assert(negative.Handoutput() == "\033[32mBNod+ -2\033[0m");  // and the hand label reads oddly

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

    // ??? UNCLEAR: the constructor takes a POSITIVE 2 but getMovementAmount() gives back
    // -2. Nothing in the design says which sign the caller is supposed to pass, so
    // RecoilNode(-2) would move the player FORWARD. We should pick one rule and use it
    // everywhere (same problem exists in Recoiler).
    assert(recoil.getMovementAmount() == -2);                   // backward movement is negative

    // Same split as BoostNode: the bridge label has no number, the hand label does. The
    // hand label shows the POSITIVE amount that was passed in, written after a minus sign.
    assert(recoil.output() == "\033[31mRNod-\033[0m");          // red, no number
    assert(recoil.Handoutput() == "\033[31mRNod- 2\033[0m");    // red, with the number

    RecoilNode zero(0);

    assert(zero.getMovementAmount() == 0);                      // a recoil of 0 moves nobody

    RecoilNode negative(-3);

    assert(negative.getMovementAmount() == 3);                  // a negative amount flips forward
    assert(negative.Handoutput() == "\033[31mRNod- -3\033[0m"); // and the hand label reads oddly

    RecoilNode* copy = recoil.clone();

    assert(copy != &recoil);                                 // clone makes a new object
    assert(copy->getMovementAmount() == -2);                 // and keeps the recoil amount

    delete copy;

    // A boost and a recoil of the same size cancel out.
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

    // ??? UNCLEAR: DiceNode still does not override getMovementAmount(), so it reports 0
    // and the roll only comes out of rollDice(). Two separate places now have to know
    // that on their own: Bridge::insertCard() checks with typeid, and
    // GameLogic::calculateMovement() checks with dynamic_cast. Anywhere else that asks a
    // node how far it moves a player will silently get 0.
    assert(dice.getMovementAmount() == 0);    // what it does now: always 0, never the roll

    // ??? UNCLEAR: the design document says the dice gives +5, +4, +3, +2, +1, 0, -1, -2,
    // -3, -4 and the class table says +4, +3, +2, +1, 0, -1. The code uses
    // 4, 2, 1, -1, -2, -4. All three lists are different, so we need to agree on one.

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

    // The colour decides the colour of the label. A portal is the one node whose bridge
    // label and hand label are identical, because it carries no number either way.
    assert(PortalNode("Yellow").output() == "\033[33mPortal\033[0m");        // yellow portal
    assert(PortalNode("Blue").output() == "\033[34mPortal\033[0m");          // blue portal
    assert(PortalNode("Purple").output() == "\033[35mPortal\033[0m");        // purple portal
    assert(PortalNode("Orange").output() == "\033[38;5;208mPortal\033[0m");  // orange portal
    assert(PortalNode("Green").output() == "Portal ");                       // unknown colour, no colour code
    assert(PortalNode("blue").output() == "Portal ");                        // the check is case sensitive

    assert(PortalNode("Blue").Handoutput() == PortalNode("Blue").output());   // both labels match
    assert(PortalNode("Green").Handoutput() == "Portal ");

    PortalNode* copy = blue.clone();

    assert(copy != &blue);                             // clone makes a new object
    assert(copy->getColour() == "Blue");               // with the same colour

    // !!! BROKEN: clone() copies the connectedPortal POINTER, so the copy is linked to the
    // ORIGINAL portal's partner instead of being unlinked. When a Bridge is copied its
    // portals still point into the old bridge, and GameLogic::movingPhase() looks for the
    // partner by walking the CURRENT bridge, so it never finds it and the player is never
    // teleported. clone() should set connectedPortal to nullptr and let
    // GameLogic::linkPortal() pair them up again.
    assert(copy->getConnectedPortal() == &purple);     // what it does now: points at the old partner
    // assert(copy->getConnectedPortal() == nullptr);  // what we actually want

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

    // getModifiedAmount adds the boost onto whatever the node already moves.
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

    // ??? UNCLEAR: getRecoilAmount() returns the NEGATIVE of what was passed in, but
    // getModifiedAmount() subtracts the POSITIVE stored value. Both end up correct, but
    // the two functions disagree about what "the recoil amount" means. Pick one.
    assert(recoiler.getRecoilAmount() == -2);    // 2 goes in, -2 comes out

    // getModifiedAmount takes the recoil off whatever the node already moves.
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

    // A booster and a recoiler of the same size cancel out on the same node.
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

    // getModifiedAmount multiplies whatever the node already moves.
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

    // A multiplier hits whatever the node adds up to at the point it sits in the strand,
    // not the node's own card. On a plain node already carrying a +2 booster, a x3 sees 2
    // and turns it into 6.
    Booster earlier(2);
    Multiplier tripler(3);
    int runningTotal = earlier.getModifiedAmount(0);

    assert(runningTotal == 2);                          // plain node 0, booster makes it 2

    runningTotal = tripler.getModifiedAmount(runningTotal);

    assert(runningTotal == 6);                          // then the multiplier makes it 6

    // Placement order decides everything. The same two cards the other way round give a
    // different node, which is why the strand has to stay in order.
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

    // Dynamite destroys the node instead of changing its movement, so it leaves the
    // movement alone.
    int unchanged = dynamite.getModifiedAmount(5);

    assert(unchanged == 5);                         // the movement is not touched

    ModifierCard* copy = dynamite.clone();

    assert(copy->getName() == "Dynamite");          // the copy still says "Dynamite"

    // !!! BROKEN: Dynamite still does not have its own clone(), so ModifierCard::clone()
    // runs and builds a plain ModifierCard. The name is copied but the TYPE is lost.
    // GameLogic::applyPlacement() finds dynamite with dynamic_cast<Dynamite*>, so a copied
    // bridge would hold dynamite that can never be set off.
    // Fix: add "virtual Dynamite* clone() const;" to Dynamite like the other modifiers.
    assert(dynamic_cast<Dynamite*>(copy) == nullptr);     // what it does now: the copy is not a Dynamite
    // assert(dynamic_cast<Dynamite*>(copy) != nullptr);  // what we actually want

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

    // ---- generateStartingDeck ----
    deck.generateStartingDeck();

    assert(deck.getDeckSize() == 50);                    // the deck is filled to MAX_DECK_SIZE
    assert(!deck.isEmpty());                             // so it is no longer empty
    assert(deck.getCard(0)->getName() == "Card 1");      // first card
    assert(deck.getCard(49)->getName() == "Card 50");    // last card
    assert(deck.getCard(50) == nullptr);                 // nothing past the end
    assert(deck.getCard(-1) == nullptr);                 // a negative index gives nothing

    // ??? UNCLEAR: generateStartingDeck() makes 50 cards literally named "Card 1" to
    // "Card 50" that are plain Card objects, not boost nodes or dynamite. Nothing calls it
    // any more now that GameLogic uses generateDeckForState(), so it should probably be
    // deleted. A game that used it would deal cards that cannot be played at all.

    // ---- drawCard ----
    Card* top = deck.drawCard();

    assert(top->getName() == "Card 50");        // drawCard takes from the top, which is the last index
    assert(deck.getDeckSize() == 49);           // the deck shrank by one
    assert(deck.getCard(49) == nullptr);        // and that slot is now empty

    delete top;                                 // the deck no longer owns this card, we do

    // ---- drawCardAtIndex ----
    Card* first = deck.drawCardAtIndex(0);

    assert(first->getName() == "Card 1");                // takes the card at that index
    assert(deck.getDeckSize() == 48);                    // the deck shrank by one
    assert(deck.getCard(0)->getName() == "Card 2");      // the cards behind it shift down

    delete first;

    assert(deck.drawCardAtIndex(-1) == nullptr);         // a negative index draws nothing
    assert(deck.drawCardAtIndex(48) == nullptr);         // and so does one past the last card
    assert(deck.getDeckSize() == 48);                    // a refused draw does not change the deck

    // ---- outputDeck ----
    while (deck.getDeckSize() > 2)
    {
        delete deck.drawCard();
    }

    assert(deck.outputDeck() == "Card 2\nCard 3\n");     // one card per line, in deck order

    // ---- emptying the deck ----
    while (!deck.isEmpty())
    {
        delete deck.drawCard();
    }

    assert(deck.getDeckSize() == 0);           // drawing every card empties the deck exactly
    assert(deck.isEmpty());                    // and it reports itself as empty
    assert(deck.drawCard() == nullptr);        // and it stays empty

    // ---- shuffleDeck ----
    deck.generateStartingDeck();
    deck.shuffleDeck();

    assert(deck.getDeckSize() == 50);          // shuffling does not gain or lose cards
    assert(deck.getCard(0) != nullptr);        // every slot still holds a card
    assert(deck.getCard(49) != nullptr);
    assert(deck.getCard(50) == nullptr);       // and the deck did not grow

    // ---- generateDeckForState ----
    // The start of a game: a 12 node bridge with player 1 on node 1 and player 2 on node
    // 12. That is the calmest game state, so this builds tier 0.
    deck.generateDeckForState(12, 1, 12);

    assert(deck.getDeckSize() == 50);          // every tier builds exactly 50 cards
    assert(deck.getCard(0) != nullptr);        // and they are all real cards
    assert(deck.getCard(49) != nullptr);
    assert(deck.getCard(50) == nullptr);       // the deck did not overflow

    // A longer bridge with both players further from the finish is a later, more urgent
    // state, so this builds a higher tier. Every tier still holds exactly 50 cards, and
    // deckSize is now counted by addCard() instead of being set by hand.
    deck.generateDeckForState(30, 15, 20);

    assert(deck.getDeckSize() == 50);          // regenerating clears the old deck, it does not add to it
    assert(deck.getCard(49) != nullptr);       // and every slot it claims really holds a card
    assert(deck.getCard(50) == nullptr);

    // The urgency is remembered between decks, so it can only ever go up. Asking for the
    // calmest possible state again must not drop the deck back to tier 0.
    deck.generateDeckForState(12, 1, 12);

    assert(deck.getDeckSize() == 50);          // still a full deck
    assert(deck.getCard(49) != nullptr);       // built from a real tier, not a half filled one

    // ??? UNCLEAR: each tier adds two portals of the same colour and the comment calls
    // them a "linked pair", but they are not linked here. GameLogic::linkPortal() pairs
    // them up by colour once they are placed on the bridge. Since every portal in a tier
    // has the same colour, a third portal would link to whichever one it finds first
    // walking from player 1's castle, which may not be the pair the player wanted.
}

// =====================================================================================
// Player
// =====================================================================================
void testPlayer()
{
    // The player only stores Node pointers, so plain nodes are enough to test it.
    Node castle;
    Node middle;

    Player player(1, &castle);

    assert(player.getPlayerId() == 1);              // the id given to the constructor
    assert(player.getCurrentNode() == &castle);     // a player starts on their own castle
    assert(player.getCastleNode() == &castle);      // and remembers which castle is theirs
    assert(player.getHandSize() == 0);              // with an empty hand
    assert(player.isHandFull() == false);           // that is not full
    assert(player.hasWon() == false);               // and has not won yet

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
    player.setWon(true);

    assert(player.hasWon() == true);                // the win flag can be set

    player.setWon(false);

    assert(player.hasWon() == false);               // and cleared again

    // !!! BROKEN: nothing in the game ever calls setWon(). GameLogic::checkWinCondition()
    // works out the winner and sets its own gameOver flag instead, so Player::hasWon() is
    // always false even after somebody has won. Either checkWinCondition() should call
    // setWon() on the winner, or hasWon()/setWon() should be deleted from Player.

    // ??? UNCLEAR: nobody deletes the cards in a hand. Player's destructor is empty and
    // the copy constructor copies the POINTERS, so two players can end up holding the same
    // card. GameLogic deletes dynamite itself after it goes off and hands every other card
    // to the bridge, but a card still in a hand when the game ends is leaked.
    delete cardA;
    delete cardB;
    delete cardC;
    delete cardD;
    delete cardE;
    delete cardF;

    // A plain Node starts with no card on it, so the two nodes above have nothing to
    // clean up. Bridge fills the card in itself for the nodes that live on a bridge.
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

    // The constructor puts a plain NodeCard on every node it builds, castles included.
    assert(castle1->nodeCard != nullptr);                     // even the castles hold a card
    assert(castle1->nodeCard->getName() == "Normal Node");    // a plain one
    assert(castle1->movementAmount == 0);                     // that moves nobody
    assert(castle1->beginningOfStrand == nullptr);            // and has no modifiers

    int castleMovement = bridge.getMovementOnBridge(castle1);

    assert(castleMovement == 0);                              // and the getter agrees

    // ??? UNCLEAR: giving the castles a NodeCard means they hold a card too, even though
    // the design says castles have no effect and cannot be played on. Nothing stops that
    // card being read as an effect later.

    // ---- isValidNode ----
    // Node numbers start at 1, and 1 and 12 are the castles.
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

    // ??? UNCLEAR: "valid" means two different things here. isValidNode() says the castles
    // are NOT valid, but isValidNodePlacement(1, 2) says node 1 IS a valid left neighbour.
    // That is correct for the game, but the names make it look like a bug.

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

    // A dice or a portal node keeps a movement of 0, because the amount is not known until
    // the player lands on it.
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

    // !!! BROKEN: insertCard() is never called with a null card in this test because it
    // would kill the program. It does "typeid(*newNodeCard)" twice, and dereferencing a
    // null pointer there throws std::bad_typeid.
    // Add "if (newNodeCard == nullptr) return false;" at the top.

    // ---- attachModifierCard ----
    bool attached = bridge.attachModifierCard(2, new Booster(1));

    assert(attached == true);                                                    // a modifier can be attached to a real node
    assert(inserted->beginningOfStrand != nullptr);                              // the node now has a strand
    assert(inserted->beginningOfStrand->modifierCard->getName() == "Booster");   // holding our booster
    assert(inserted->beginningOfStrand->next == nullptr);                        // and nothing after it

    // !!! BROKEN: the FIRST modifier put on a node never changes the node's movement. When
    // the strand is empty attachModifierCard() takes the early "return true;" branch, which
    // sits ABOVE the line that does
    //     tempptr->movementAmount = modifier->getModifiedAmount(tempptr->movementAmount);
    // so that line only ever runs for the second modifier onwards. The booster just
    // attached is worth +1 and the node still says 3.
    // Fix: move the movementAmount update above the early return, or drop the early return
    // and let the append loop handle the empty case.
    assert(inserted->movementAmount == 3);        // what it does now: the booster was ignored
    // assert(inserted->movementAmount == 4);     // what we actually want: 3 + 1

    bridge.attachModifierCard(2, new Multiplier(2));

    assert(inserted->beginningOfStrand->modifierCard->getName() == "Booster");            // the first modifier stays first
    assert(inserted->beginningOfStrand->next->modifierCard->getName() == "Multiplier");   // the new one goes on the end

    // The second modifier IS applied, on top of the amount that skipped the first one:
    // 3 (booster never counted) * 2 = 6.
    assert(inserted->movementAmount == 6);              // what it does now
    assert(bridge.getMovementOnBridge(inserted) == 6);  // and the getter agrees
    // assert(inserted->movementAmount == 8);           // what this line would say if the
                                                        // first modifier were not skipped

    // THE RULE: a node's movement is worked out by walking the modifier strand from the
    // first card placed to the last, applying each one to the running total. The strand is
    // in placement order, so the order the cards were played in is the order they apply in.
    // "Multipliers go first" is NOT a general rule: it only breaks the tie when both
    // players attach a modifier to the SAME node on the SAME turn, and it decides which of
    // those two goes into the strand first. Everything else is just placement order.
    //
    // So for this node, BoostNode(+3) with strand [Booster +1, Multiplier x2]:
    //     3 + 1 = 4, then 4 * 2 = 8.  The right answer is 8.
    //
    // Bridge::attachModifierCard() already works this way, applying each card as it lands.
    // It is the right model, and once the skipped first modifier above is fixed it will
    // give 8 on its own.

    // Placing the same two cards in the other order must give a different answer, because
    // the strand order is different: 3 * 2 = 6, then 6 + 1 = 7.
    Bridge orderCheck;

    orderCheck.insertCard(1, 2, new BoostNode(3));
    orderCheck.attachModifierCard(2, new Multiplier(2));    // multiplier lands first this time
    orderCheck.attachModifierCard(2, new Booster(1));       // then the booster

    Node* orderedNode = orderCheck.getPlayer1Castle()->right;

    assert(orderedNode->beginningOfStrand->modifierCard->getName() == "Multiplier");   // strand order is kept
    assert(orderedNode->beginningOfStrand->next->modifierCard->getName() == "Booster");
    assert(orderCheck.getMovementOnBridge(orderedNode) == 4);   // what it does now: the x2 was the
                                                                // skipped first modifier, so 3 + 1
    // assert(orderCheck.getMovementOnBridge(orderedNode) == 7);   // what we want: 3 * 2 + 1

    // !!! BROKEN: GameLogic::calculateMovement() does not follow the rule at all. It walks
    // the strand twice, adding every booster and recoiler in the first pass and applying
    // every multiplier in the second, so it always behaves as if every multiplier were
    // placed last no matter what order the cards were really played in. For the node just
    // built it returns (3 + 1) * 2 = 8 where the strand says 3 * 2 + 1 = 7.
    // The fix is smaller than the current code: getModifiedAmount() is virtual precisely so
    // that the caller does not have to know which kind of modifier it is holding. Replace
    // both while loops and all four dynamic_casts with one walk:
    //     ModifierStrand* m = node->beginningOfStrand;
    //     while (m != nullptr) {
    //         if (m->modifierCard != nullptr) movement = m->modifierCard->getModifiedAmount(movement);
    //         m = m->next;
    //     }
    // That is the same loop Bridge::attachModifierCard() already does one card at a time,
    // so the two will finally agree.

    // A worked example from the rules, played out on a real bridge. A plain node starts at
    // 0, then over three turns:
    //     turn 1  p1 attaches Booster +2      0 + 2  =  2
    //     turn 2  p1 attaches Multiplier x3   2 * 3  =  6
    //     turn 3  p1 attaches Booster +1 and p2 attaches Multiplier x2 at the same time,
    //             so the multiplier goes into the strand first:
    //                                         6 * 2  = 12
    //                                        12 + 1  = 13
    // The node should end up at +13.
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

    // !!! BROKEN: the answer is 1, not 13. The +2 on turn 1 was the first modifier on the
    // node so it was thrown away, leaving 0. Then 0 * 3 = 0, 0 * 2 = 0, 0 + 1 = 1. The
    // skipped first modifier is worse than losing one card: because a multiplier follows
    // it, the whole node collapses to almost nothing.
    assert(worked.getMovementOnBridge(workedNode) == 1);      // what it does now
    // assert(worked.getMovementOnBridge(workedNode) == 13);  // what the rules say

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

    // A copied bridge gets its own card on every node, castles included, the same way a
    // fresh bridge does.
    assert(copy.getPlayer1Castle()->nodeCard != nullptr);                         // the castles get a card
    assert(copy.getPlayer2Castle()->nodeCard != nullptr);
    assert(copy.getPlayer1Castle()->nodeCard != castle1->nodeCard);               // a new one, not shared
    assert(copy.getPlayer1Castle()->nodeCard->getName() == "Normal Node");        // and a plain one

    // The middle nodes are copied properly too: new cards, not shared pointers.
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

    // A 2 node bridge copies correctly. copyFrom() runs its loop once per node BETWEEN the
    // castles, then adds the far castle, so the copy is always 1 + (n - 2) + 1 = n nodes.
    // With no middle nodes the loop just does not run.
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

    // Stand player 1 on the boost node so the player marker can be seen.
    player1.setCurrentNode(bridge.getPlayer1Castle()->right);

    Card* handCard1 = new BoostNode(3);
    Card* handCard2 = new Dynamite();

    player1.addCardToHand(handCard1);
    player1.addCardToHand(handCard2);

    Visual visual;

    // Should print 16 nodes separated by " --- ", each followed by its number, and should
    // break onto a new line after every 6th node.
    // Node  1 is "[P1's| Castle |] 1" and node 16 is "[P2's| Castle |]16".
    // Node  2 is the boost node with player 1 on it, so "[|" then a blue "P1" then "|"
    //          then a green "BNod+" then " 4  ]". The booster was the first modifier on
    //          that node so it was skipped (see testBridge), leaving 2 * 2 = 4.
    // Node  3 is the recoil node:  "[|  |" red "RNod-" " -1 ]"
    // Node  4 is the blue portal:  "[|  |" blue "Portal" " 0  ]"
    // Node  5 is the dice node:    "[|  |Dice   0  ]"
    // Nodes 6 to 15 are plain:     "[|  |Norm    0  ]"
    std::cout << "printBridge:" << std::endl;
    visual.printBridge(bridge, player1, player2);
    std::cout << std::endl;

    // Should print two lines, the first in blue, the second in red:
    //   P 1's Hand: BNod+ 3, Dynamite
    //   P 2's Hand: [Empty Hand]
    std::cout << "printHands:" << std::endl;
    visual.printHands(player1, player2);

    delete handCard1;
    delete handCard2;

    // !!! BROKEN: printBridge() still does not show which modifiers are on a node. Node 2
    // above carries a booster and a multiplier, and all a player sees is the total, 6. They
    // cannot tell whether it is safe to add a recoiler, or that a multiplier is waiting to
    // double whatever they attach next. Knowing what is stacked on a node is most of the
    // strategy in this game. The comment above printNode() in Visual.hpp also still
    // describes a printModifiers() helper that no longer exists.

    // !!! BROKEN: the number printed next to a node is Node::movementAmount, which is the
    // cached total from Bridge. The moving phase uses GameLogic::calculateMovement()
    // instead, and the two disagree whenever a multiplier was attached before a booster
    // (see testBridge). So the bridge can show a player one number and then move them by a
    // different one.

    // ??? UNCLEAR: printBridge() prints every node on one long line. The design says it
    // would wrap onto the next line once it gets too wide. Because the node labels are
    // different widths (see testNodeCard) the numbers after them do not line up either.
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

    // !!! BROKEN: startGame() is not called here on purpose. It sits in a while loop doing
    // "std::cin >> choice" and never checks whether the read worked, and placingPhase()
    // does the same with "std::cin >> choice.cardIndex" inside its own while(true). If the
    // input runs out the reads fail, the variables keep their old values, and the loops
    // spin forever. Calling it from a test would hang the whole test run.
    // Fix: check std::cin after every read and treat a failed read as quitting.

    // !!! BROKEN: resolvePlacements() puts the two cards in the wrong order when both
    // players choose the same gap. It calls applyPlacement(p2choice) first and then
    // applyPlacement(p1choice), and insertCard() always drops the new node immediately to
    // the RIGHT of leftNode, so player 1's card ends up to the LEFT of player 2's. The
    // design says the left sided player (P2) goes left and the right sided player (P1)
    // goes right, so those two calls need swapping.

    // !!! BROKEN: resolvePlacements() does not apply the multiplier-first tie-break either.
    // When both players attach a modifier to the SAME node on the SAME turn, a multiplier
    // has to go into the strand before a booster or a recoiler, because the strand order is
    // what decides the maths (see testBridge). Two modifiers on one node currently fall
    // through to the last two lines, which always do p1 then p2, so whoever is player 1
    // wins the tie instead of the multiplier. Taking the worked example from testBridge, a
    // node sitting at +6 where p1 plays Booster +1 and p2 plays Multiplier x2:
    //     what it does now:   6 + 1 = 7, then 7 * 2 = 14
    //     what the rules say: 6 * 2 = 12, then 12 + 1 = 13
    // Add a check next to the two-dynamite case: if both cards are modifiers aimed at the
    // same targetNode and exactly one is a Multiplier, apply that one first.

    // !!! BROKEN: drawingPhase() deals a plain NodeCard when the deck runs out. The design
    // says the game enters demolition mode and deals only dynamite until the bridge is
    // back down to 10 nodes, then builds a new deck. Neither happens, so an empty deck
    // just hands out blank nodes forever and the game can never end that way. It also
    // draws only one card in that case instead of three, and does not check whether the
    // hand is full first, so the card leaks if it is.

    // ??? UNCLEAR: checkWinCondition() decides the winner and sets gameOver, but never
    // calls setWon() on the winning Player (see testPlayer).

    // ??? UNCLEAR: movingPhase() stops the chain the moment a player teleports through a
    // portal, so the node they land on never activates. The design says a player keeps
    // moving until they land on a node with no movement effect or one they have already
    // used this turn, which suggests the chain should carry on after a teleport.

    // ??? UNCLEAR: startGame() builds the deck once with generateDeckForState(12, 1, 12)
    // and never builds another one, so the tier system that Deck was written for is only
    // ever asked for tier 0. The deck should be rebuilt from the real bridge size and the
    // real player positions when it runs out.
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

    // Printed last because it writes to the screen instead of asserting.
    testVisual();

    return 0;
}
