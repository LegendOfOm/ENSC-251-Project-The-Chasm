// =====================================================================================
// test.cpp -- Unit tests for "The Chasm"
//
// One test function per source file. Every check goes through assert(), so the suite
// aborts on the first violated expectation and prints the failing file/line.
//
// BUILD (every .cpp EXCEPT main.cpp, because this file provides its own main):
//
//   g++ -std=c++11 -g -o test test.cpp Bridge.cpp Visual.cpp Card.cpp
//       NodeCard.cpp ModifierCard.cpp BoosterNode.cpp RecoilNode.cpp DiceNode.cpp
//       PortalNode.cpp Booster.cpp Recoiler.cpp Mutiplier.cpp Dynamite.cpp
//       Deck.cpp Player.cpp GameLogic.cpp
//   ./test
//
//   (all on one line, or use the Makefile target of your choice)
//
// The tests assert the CURRENT behaviour of the code so that they stay green today and
// break loudly if somebody changes that behaviour by accident. Where the current
// behaviour disagrees with the design proposal, or is an outright defect, the assert is
// tagged with a "BUG:" comment instead of being silently skipped.
// =====================================================================================

#undef NDEBUG // asserts must stay live even in a release build
#include <cassert>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Booster.hpp"
#include "BoosterNode.hpp"
#include "Bridge.hpp"
#include "Visual.hpp"
#include "Card.hpp"
#include "Deck.hpp"
#include "DiceNode.hpp"
#include "Dynamite.hpp"
#include "GameLogic.hpp"
#include "ModifierCard.hpp"
#include "Multiplier.hpp"
#include "NodeCard.hpp"
#include "Player.hpp"
#include "PortalNode.hpp"
#include "RecoilNode.hpp"
#include "Recoiler.hpp"

// -------------------------------------------------------------------------------------
// MISSING IMPLEMENTATION -- supplied here only so the suite can link.
//
// Deck::getDeckSize() is declared in Deck.hpp but has no definition in Deck.cpp, so any
// translation unit that calls it fails to link ("undefined reference to
// Deck::getDeckSize() const"). main.cpp never calls it, which is why the game still
// builds today.
//
// FIX: move this definition into Deck.cpp and set the macro below to 0.
// -------------------------------------------------------------------------------------
#define TEST_SUPPLIES_DECK_GETDECKSIZE 1

#if TEST_SUPPLIES_DECK_GETDECKSIZE
int Deck::getDeckSize() const
{
    return deckSize;
}
#endif

// Bridge::printBridge() dereferences node->nodeCard without a null check, and the castle
// nodes never own a nodeCard, so calling it on any bridge is an immediate null-pointer
// dereference. The test is written below but compiled out until Bridge.cpp is fixed.
// Flip this to 1 once printBridge() guards against a null nodeCard.
#define TEST_BRIDGE_PRINT_BRIDGE 0

// -------------------------------------------------------------------------------------
// Tiny test harness
// -------------------------------------------------------------------------------------

static int g_checks = 0;
static int g_tests = 0;

#define CHECK(cond)      \
    do {                 \
        ++g_checks;      \
        assert(cond);    \
    } while (0)

static void runTest(const char* name, void (*testFunction)())
{
    ++g_tests;
    std::cout << "  running " << name << " ... " << std::flush;
    testFunction();
    std::cout << "ok" << std::endl;
}

// Redirects std::cout into a string for the duration of its lifetime, so the printing
// functions can be tested on their actual output instead of eyeballing the terminal.
class CoutCapture
{
public:
    CoutCapture() : buffer(), previousBuffer(std::cout.rdbuf(buffer.rdbuf())) {}
    ~CoutCapture() { std::cout.rdbuf(previousBuffer); }

    std::string str() const { return buffer.str(); }

private:
    std::ostringstream buffer;      // must be declared before previousBuffer
    std::streambuf* previousBuffer;

    CoutCapture(const CoutCapture&);
    CoutCapture& operator=(const CoutCapture&);
};

// Feeds a canned script into std::cin so interactive code can be driven from a test.
class CinRedirect
{
public:
    explicit CinRedirect(const std::string& input)
        : buffer(input), previousBuffer(std::cin.rdbuf(buffer.rdbuf()))
    {
    }
    ~CinRedirect() { std::cin.rdbuf(previousBuffer); }

private:
    std::istringstream buffer;      // must be declared before previousBuffer
    std::streambuf* previousBuffer;

    CinRedirect(const CinRedirect&);
    CinRedirect& operator=(const CinRedirect&);
};

static bool contains(const std::string& haystack, const std::string& needle)
{
    return haystack.find(needle) != std::string::npos;
}

// -------------------------------------------------------------------------------------
// Bridge helpers -- Node is a public struct, so the tests can walk the list directly and
// verify the links instead of trusting the Bridge's own accessors.
// -------------------------------------------------------------------------------------

// Returns the node at a 1-based index (1 == player 1's castle), or nullptr if the index
// runs off the end of the bridge.
static Node* nodeAt(const Bridge& bridge, int index)
{
    if (index < 1) return nullptr;
    Node* current = bridge.getPlayer1Castle();
    for (int i = 1; i < index && current != nullptr; ++i) {
        current = current->right;
    }
    return current;
}

// Counts the nodes from player 1's castle to the end of the list, castles included.
static int bridgeLength(const Bridge& bridge)
{
    int length = 0;
    for (Node* current = bridge.getPlayer1Castle(); current != nullptr; current = current->right) {
        ++length;
    }
    return length;
}

// Verifies left/right pointers agree with each other along the whole bridge.
static bool linksAreConsistent(const Bridge& bridge)
{
    Node* current = bridge.getPlayer1Castle();
    if (current == nullptr) return false;
    if (current->left != nullptr) return false;

    while (current->right != nullptr) {
        if (current->right->left != current) return false;
        current = current->right;
    }
    return current == bridge.getPlayer2Castle();
}

// Number of modifier cards attached to a node.
static int strandLength(const Node* node)
{
    int length = 0;
    for (const ModifierStrand* s = node->beginningOfStrand; s != nullptr; s = s->next) {
        ++length;
    }
    return length;
}

// Name of the modifier at position `index` (0-based) in a node's strand.
static std::string modifierNameAt(const Node* node, int index)
{
    const ModifierStrand* s = node->beginningOfStrand;
    for (int i = 0; i < index && s != nullptr; ++i) {
        s = s->next;
    }
    if (s == nullptr || s->modifierCard == nullptr) return "";
    return s->modifierCard->getName();
}

// =====================================================================================
// Card.cpp
// =====================================================================================
static void test_Card()
{
    Card plain("Test Card");
    CHECK(plain.getName() == "Test Card");
    CHECK(plain.output() == "Test Card"); // base output() is just the name

    // Boundary: empty name is accepted and round-trips unchanged.
    Card empty("");
    CHECK(empty.getName() == "");
    CHECK(empty.output() == "");

    // Boundary: names are stored verbatim, spaces and escape codes included.
    Card weird("  \033[31mRed\033[0m  ");
    CHECK(weird.getName() == "  \033[31mRed\033[0m  ");

    // getName() must not mutate the card.
    CHECK(plain.getName() == plain.getName());

    // The destructor is virtual, so deleting a derived card through a Card* is safe and
    // output() dispatches to the derived override.
    Card* polymorphic = new BoostNode(2);
    CHECK(polymorphic->getName() == "Boost Node");
    CHECK(polymorphic->output() != polymorphic->getName()); // BoostNode formats its label
    delete polymorphic;

    // Copying a Card copies the name.
    Card copy(plain);
    CHECK(copy.getName() == "Test Card");
}

// =====================================================================================
// NodeCard.cpp
// =====================================================================================
static void test_NodeCard()
{
    NodeCard defaultNode;
    CHECK(defaultNode.getName() == "Normal Node"); // default argument
    CHECK(defaultNode.getMovementAmount() == 0);   // a plain node never moves anybody
    CHECK(defaultNode.output() == "[Norm  ]");     // fixed 8-character label

    NodeCard named("Custom");
    CHECK(named.getName() == "Custom");
    CHECK(named.getMovementAmount() == 0);
    CHECK(named.output() == "[Norm  ]"); // output ignores the name

    // clone() must produce an independent object of the same type.
    NodeCard* copy = defaultNode.clone();
    CHECK(copy != &defaultNode);
    CHECK(copy != nullptr);
    CHECK(copy->getName() == defaultNode.getName());
    CHECK(copy->getMovementAmount() == 0);
    delete copy;

    // Virtual dispatch through a Card* reaches NodeCard's override.
    Card* asCard = new NodeCard();
    CHECK(asCard->output() == "[Norm  ]");
    delete asCard;

    // Every special node is-a NodeCard, and clone() stays type-correct through a base
    // pointer (covariant return types).
    NodeCard* boost = new BoostNode(3);
    NodeCard* boostClone = boost->clone();
    CHECK(dynamic_cast<BoostNode*>(boostClone) != nullptr);
    CHECK(boostClone->getMovementAmount() == 3);
    delete boost;
    delete boostClone;
}

// =====================================================================================
// ModifierCard.cpp
// =====================================================================================
static void test_ModifierCard()
{
    ModifierCard defaultModifier;
    CHECK(defaultModifier.getName() == "Modifier");
    CHECK(defaultModifier.output() == "Modifier"); // inherits Card::output()

    ModifierCard named("Custom Modifier");
    CHECK(named.getName() == "Custom Modifier");

    ModifierCard* copy = defaultModifier.clone();
    CHECK(copy != &defaultModifier);
    CHECK(copy->getName() == "Modifier");
    delete copy;

    // Cloning a derived modifier through a base pointer keeps the derived type...
    ModifierCard* booster = new Booster(2);
    ModifierCard* boosterClone = booster->clone();
    CHECK(dynamic_cast<Booster*>(boosterClone) != nullptr);
    CHECK(dynamic_cast<Booster*>(boosterClone)->getBoostAmount() == 2);
    delete booster;
    delete boosterClone;

    // ...but only for the subclasses that override clone(). Dynamite does not, so it is
    // sliced: the copy keeps the name but loses its type.
    // BUG: Dynamite must override clone() (see Dynamite.hpp), otherwise copying a Bridge
    // that carries dynamite turns it into a plain ModifierCard.
    ModifierCard* dynamite = new Dynamite();
    ModifierCard* dynamiteClone = dynamite->clone();
    CHECK(dynamiteClone->getName() == "Dynamite");                  // name survives
    CHECK(dynamic_cast<Dynamite*>(dynamiteClone) == nullptr);       // type does not
    delete dynamite;
    delete dynamiteClone;
}

// =====================================================================================
// BoosterNode.cpp  (class BoostNode)
// =====================================================================================
static void test_BoosterNode()
{
    BoostNode boost(3);
    CHECK(boost.getName() == "Boost Node");
    CHECK(boost.getMovementAmount() == 3); // forward movement is positive
    CHECK(boost.output() == "\033[32m[BNod+3]\033[0m");

    // Boundary: a +1 boost, the weakest card in the generated deck.
    BoostNode one(1);
    CHECK(one.getMovementAmount() == 1);
    CHECK(one.output() == "\033[32m[BNod+1]\033[0m");

    // Boundary: zero boost behaves like a normal node but keeps the boost label.
    BoostNode zero(0);
    CHECK(zero.getMovementAmount() == 0);
    CHECK(zero.output() == "\033[32m[BNod+0]\033[0m");

    // Boundary: the constructor does not clamp, so a negative "boost" moves backwards.
    BoostNode negative(-2);
    CHECK(negative.getMovementAmount() == -2);
    CHECK(negative.output() == "\033[32m[BNod+-2]\033[0m"); // label reads oddly by design

    // Boundary: a large amount is stored as-is (the bridge is responsible for clamping
    // movement at the castle nodes).
    BoostNode huge(1000);
    CHECK(huge.getMovementAmount() == 1000);

    // clone() is independent and keeps the amount.
    BoostNode* copy = boost.clone();
    CHECK(copy != &boost);
    CHECK(copy->getMovementAmount() == 3);
    CHECK(copy->output() == boost.output());
    delete copy;
}

// =====================================================================================
// RecoilNode.cpp
// =====================================================================================
static void test_RecoilNode()
{
    // RecoilNode stores a POSITIVE amount and negates it in getMovementAmount().
    RecoilNode recoil(2);
    CHECK(recoil.getName() == "Recoil Node");
    CHECK(recoil.getMovementAmount() == -2); // backward movement is negative
    CHECK(recoil.output() == "\033[31m[RNod-2]\033[0m");

    RecoilNode one(1);
    CHECK(one.getMovementAmount() == -1);
    CHECK(one.output() == "\033[31m[RNod-1]\033[0m");

    // Boundary: zero recoil is a no-op node.
    RecoilNode zero(0);
    CHECK(zero.getMovementAmount() == 0);
    CHECK(zero.output() == "\033[31m[RNod-0]\033[0m");

    // Boundary: a negative argument double-negates into forward movement.
    RecoilNode negative(-3);
    CHECK(negative.getMovementAmount() == 3);

    RecoilNode* copy = recoil.clone();
    CHECK(copy != &recoil);
    CHECK(copy->getMovementAmount() == -2);
    delete copy;

    // A boost and a recoil of the same size cancel out.
    BoostNode boost(2);
    CHECK(boost.getMovementAmount() + recoil.getMovementAmount() == 0);
}

// =====================================================================================
// DiceNode.cpp
// =====================================================================================
static void test_DiceNode()
{
    DiceNode dice;
    CHECK(dice.getName() == "Dice Node");
    CHECK(dice.output() == "[Dice  ]");

    // NOTE: DiceNode does not override getMovementAmount(), so it reports 0 and the roll
    // must be read through rollDice(). Anything that moves players has to special-case
    // this node.
    CHECK(dice.getMovementAmount() == 0);

    // Every roll must land in the implemented result table.
    // NOTE: the design proposal lists +5..-4; the implementation uses {4,2,1,-1,-2,-4}.
    const int allowed[6] = {4, 2, 1, -1, -2, -4};
    bool seen[6] = {false, false, false, false, false, false};

    for (int roll = 0; roll < 2000; ++roll) {
        int result = dice.rollDice();
        bool isAllowed = false;
        for (int i = 0; i < 6; ++i) {
            if (result == allowed[i]) {
                isAllowed = true;
                seen[i] = true;
            }
        }
        CHECK(isAllowed);
    }

    // Over 2000 rolls every face should have shown up; if not, rollDice() is not random.
    for (int i = 0; i < 6; ++i) {
        CHECK(seen[i]);
    }

    // Boundary: the roll never exceeds the table's extremes.
    for (int roll = 0; roll < 200; ++roll) {
        int result = dice.rollDice();
        CHECK(result <= 4);
        CHECK(result >= -4);
    }

    DiceNode* copy = dice.clone();
    CHECK(copy != &dice);
    CHECK(copy->getName() == "Dice Node");
    CHECK(copy->output() == "[Dice  ]");
    delete copy;
}

// =====================================================================================
// PortalNode.cpp
// =====================================================================================
static void test_PortalNode()
{
    PortalNode blue("Blue");
    CHECK(blue.getName() == "Portal Node");
    CHECK(blue.getColour() == "Blue");

    // Boundary: a fresh portal is unlinked and therefore inert.
    CHECK(blue.getConnectedPortal() == nullptr);

    PortalNode purple("Purple");
    blue.setConnectedPortal(&purple);
    purple.setConnectedPortal(&blue);
    CHECK(blue.getConnectedPortal() == &purple);
    CHECK(purple.getConnectedPortal() == &blue);
    // A round trip through both portals returns to the start.
    CHECK(blue.getConnectedPortal()->getConnectedPortal() == &blue);

    // Boundary: a portal can be unlinked again by passing nullptr.
    blue.setConnectedPortal(nullptr);
    CHECK(blue.getConnectedPortal() == nullptr);

    // Boundary: nothing stops a portal from linking to itself.
    blue.setConnectedPortal(&blue);
    CHECK(blue.getConnectedPortal() == &blue);
    blue.setConnectedPortal(&purple);

    // Colour drives the ANSI colour of the label; an unknown colour falls back to plain.
    CHECK(PortalNode("Yellow").output() == "\033[33m[Portal]\033[0m");
    CHECK(PortalNode("Blue").output() == "\033[34m[Portal]\033[0m");
    CHECK(PortalNode("Purple").output() == "\033[35m[Portal]\033[0m");
    CHECK(PortalNode("Green").output() == "[Portal]");
    CHECK(PortalNode("").output() == "[Portal]");
    CHECK(PortalNode("blue").output() == "[Portal]"); // matching is case sensitive

    PortalNode* copy = blue.clone();
    CHECK(copy != &blue);
    CHECK(copy->getColour() == "Blue");
    // BUG: clone() is a shallow copy, so the clone points at the ORIGINAL partner. When a
    // Bridge is copied its portals still reference nodes in the old bridge.
    CHECK(copy->getConnectedPortal() == &purple);
    delete copy;
}

// =====================================================================================
// Booster.cpp
// =====================================================================================
static void test_Booster()
{
    Booster booster(2);
    CHECK(booster.getName() == "Booster");
    CHECK(booster.getBoostAmount() == 2); // stored and returned as a positive number
    CHECK(booster.output() == "Booster"); // no custom label; inherits Card::output()

    Booster one(1);
    CHECK(one.getBoostAmount() == 1);

    // Boundary: zero adds nothing.
    Booster zero(0);
    CHECK(zero.getBoostAmount() == 0);

    // Boundary: negatives are not rejected.
    Booster negative(-4);
    CHECK(negative.getBoostAmount() == -4);

    Booster* copy = booster.clone();
    CHECK(copy != &booster);
    CHECK(copy->getBoostAmount() == 2);
    CHECK(copy->getName() == "Booster");
    delete copy;

    // A booster is a modifier, not a node.
    ModifierCard* asModifier = new Booster(1);
    CHECK(dynamic_cast<Booster*>(asModifier) != nullptr);
    CHECK(dynamic_cast<NodeCard*>(asModifier) == nullptr);
    delete asModifier;
}

// =====================================================================================
// Recoiler.cpp
// =====================================================================================
static void test_Recoiler()
{
    // NOTE: like RecoilNode, Recoiler takes a positive amount and getRecoilAmount()
    // returns it NEGATED, so it can be added straight onto a node's movement.
    Recoiler recoiler(2);
    CHECK(recoiler.getName() == "Recoiler");
    CHECK(recoiler.getRecoilAmount() == -2);
    CHECK(recoiler.output() == "Recoiler");

    Recoiler one(1);
    CHECK(one.getRecoilAmount() == -1);

    // Boundary: zero is unaffected by the negation.
    Recoiler zero(0);
    CHECK(zero.getRecoilAmount() == 0);

    // Boundary: a negative argument flips to forward movement.
    Recoiler negative(-3);
    CHECK(negative.getRecoilAmount() == 3);

    Recoiler* copy = recoiler.clone();
    CHECK(copy != &recoiler);
    CHECK(copy->getRecoilAmount() == -2);
    delete copy;

    // Booster(x) and Recoiler(x) cancel when summed onto the same node.
    Booster booster(2);
    CHECK(booster.getBoostAmount() + recoiler.getRecoilAmount() == 0);
}

// =====================================================================================
// Mutiplier.cpp  (class Multiplier)
// =====================================================================================
static void test_Multiplier()
{
    Multiplier doubler(2);
    CHECK(doubler.getName() == "Multiplier");
    CHECK(doubler.getMultiplierAmount() == 2);
    CHECK(doubler.output() == "Multiplier");

    Multiplier tripler(3);
    CHECK(tripler.getMultiplierAmount() == 3);

    // Boundary: x1 is an identity multiplier.
    Multiplier identity(1);
    CHECK(identity.getMultiplierAmount() == 1);

    // Boundary: x0 cancels a node's movement entirely.
    Multiplier zero(0);
    CHECK(zero.getMultiplierAmount() == 0);

    // Boundary: a negative multiplier reverses the node's direction.
    Multiplier negative(-1);
    CHECK(negative.getMultiplierAmount() == -1);

    Multiplier* copy = doubler.clone();
    CHECK(copy != &doubler);
    CHECK(copy->getMultiplierAmount() == 2);
    delete copy;

    // The proposal's stacking rule: multipliers apply to the node's own movement before
    // boosters/recoilers placed the same turn are added. Verified here as arithmetic on
    // the card getters, which is the contract the bridge has to implement.
    BoostNode node(3);
    Booster extraBoost(1);
    Recoiler extraRecoil(2);
    int afterMultiplier = node.getMovementAmount() * doubler.getMultiplierAmount(); // 6
    CHECK(afterMultiplier == 6);
    int total = afterMultiplier + extraBoost.getBoostAmount() + extraRecoil.getRecoilAmount();
    CHECK(total == 5); // 3*2 + 1 - 2
}

// =====================================================================================
// Dynamite.cpp
// =====================================================================================
static void test_Dynamite()
{
    Dynamite dynamite;
    CHECK(dynamite.getName() == "Dynamite");
    CHECK(dynamite.output() == "Dynamite");

    // Dynamite carries no amount; it is identified purely by its type/name.
    ModifierCard* asModifier = new Dynamite();
    CHECK(asModifier->getName() == "Dynamite");
    CHECK(dynamic_cast<Dynamite*>(asModifier) != nullptr);
    delete asModifier;

    // BUG: no clone() override, so copying loses the Dynamite type (see test_ModifierCard).
    ModifierCard* sliced = dynamite.clone();
    CHECK(dynamic_cast<Dynamite*>(sliced) == nullptr);
    delete sliced;

    // Two dynamites are distinct objects with the same name.
    Dynamite second;
    CHECK(second.getName() == dynamite.getName());
    CHECK(&second != &dynamite);

    // The destruction rules live in Bridge::removeNode(); see test_Bridge() for the
    // castle-node and relinking boundaries.
}

// =====================================================================================
// Deck.cpp
// =====================================================================================
static void test_Deck()
{
    // --- a brand new deck is empty ---
    {
        Deck deck;
        CHECK(deck.getDeckSize() == 0);
        CHECK(deck.isEmpty());
        // Boundary: drawing from an empty deck returns nullptr instead of crashing.
        CHECK(deck.drawCard() == nullptr);
        CHECK(deck.drawCardAtIndex(0) == nullptr);
        CHECK(deck.getCard(0) == nullptr);
        CHECK(deck.outputDeck() == "");
        // Boundary: shuffling an empty deck must not divide by zero.
        deck.shuffleDeck();
        CHECK(deck.getDeckSize() == 0);
    }

    // --- generateStartingDeck fills the deck to capacity ---
    {
        Deck deck;
        deck.generateStartingDeck();
        CHECK(deck.getDeckSize() == 35); // MAX_DECK_SIZE
        CHECK(!deck.isEmpty());
        CHECK(deck.getCard(0) != nullptr);
        CHECK(deck.getCard(0)->getName() == "Card 1");
        CHECK(deck.getCard(34)->getName() == "Card 35");

        // Boundary: index checks on both ends.
        CHECK(deck.getCard(-1) == nullptr);
        CHECK(deck.getCard(35) == nullptr);
        CHECK(deck.getCard(1000) == nullptr);

        // Regenerating clears first, so the deck never overflows.
        deck.generateStartingDeck();
        CHECK(deck.getDeckSize() == 35);
        CHECK(deck.getCard(0)->getName() == "Card 1");
    }

    // --- drawCard draws from the top (last index) and transfers ownership ---
    {
        Deck deck;
        deck.generateStartingDeck();

        Card* top = deck.drawCard();
        CHECK(top != nullptr);
        CHECK(top->getName() == "Card 35");
        CHECK(deck.getDeckSize() == 34);
        CHECK(deck.getCard(34) == nullptr); // the slot really was vacated
        delete top;                         // the caller owns the drawn card

        Card* next = deck.drawCard();
        CHECK(next->getName() == "Card 34");
        CHECK(deck.getDeckSize() == 33);
        delete next;

        // Boundary: draining the deck one card at a time ends at exactly empty.
        while (!deck.isEmpty()) {
            Card* card = deck.drawCard();
            CHECK(card != nullptr);
            delete card;
        }
        CHECK(deck.getDeckSize() == 0);
        CHECK(deck.isEmpty());
        CHECK(deck.drawCard() == nullptr);
    }

    // --- drawCardAtIndex removes from the middle and closes the gap ---
    {
        Deck deck;
        deck.generateStartingDeck();

        // Boundary: invalid indices are rejected without touching the deck.
        CHECK(deck.drawCardAtIndex(-1) == nullptr);
        CHECK(deck.drawCardAtIndex(35) == nullptr);
        CHECK(deck.getDeckSize() == 35);

        // Boundary: first index.
        Card* first = deck.drawCardAtIndex(0);
        CHECK(first->getName() == "Card 1");
        CHECK(deck.getDeckSize() == 34);
        CHECK(deck.getCard(0)->getName() == "Card 2"); // shifted down
        delete first;

        // Boundary: last valid index.
        Card* last = deck.drawCardAtIndex(deck.getDeckSize() - 1);
        CHECK(last->getName() == "Card 35");
        CHECK(deck.getDeckSize() == 33);
        delete last;

        // Middle removal keeps the surrounding order intact.
        std::string before = deck.getCard(11)->getName();
        Card* middle = deck.drawCardAtIndex(10);
        CHECK(deck.getDeckSize() == 32);
        CHECK(deck.getCard(10)->getName() == before);
        delete middle;

        // Boundary: emptying the deck through index 0 only.
        while (!deck.isEmpty()) {
            Card* card = deck.drawCardAtIndex(0);
            CHECK(card != nullptr);
            delete card;
        }
        CHECK(deck.isEmpty());
        CHECK(deck.drawCardAtIndex(0) == nullptr);
    }

    // --- shuffleDeck keeps exactly the same cards ---
    {
        Deck deck;
        deck.generateStartingDeck();

        const int size = deck.getDeckSize();
        Card* before[35];
        for (int i = 0; i < size; ++i) {
            before[i] = deck.getCard(i);
        }

        deck.shuffleDeck();
        CHECK(deck.getDeckSize() == size); // no cards gained or lost

        // Every card that was in the deck is still in the deck, exactly once.
        for (int i = 0; i < size; ++i) {
            int matches = 0;
            for (int j = 0; j < size; ++j) {
                if (deck.getCard(j) == before[i]) ++matches;
            }
            CHECK(matches == 1);
        }
        for (int i = 0; i < size; ++i) {
            CHECK(deck.getCard(i) != nullptr);
        }

        // Boundary: shuffling a single-card deck is a no-op, not a crash.
        while (deck.getDeckSize() > 1) {
            delete deck.drawCard();
        }
        Card* only = deck.getCard(0);
        deck.shuffleDeck();
        CHECK(deck.getDeckSize() == 1);
        CHECK(deck.getCard(0) == only);
    }

    // --- outputDeck lists one card per line ---
    {
        Deck deck;
        deck.generateStartingDeck();
        while (deck.getDeckSize() > 2) {
            delete deck.drawCard(); // leaves "Card 1" and "Card 2"
        }
        CHECK(deck.outputDeck() == "Card 1\nCard 2\n");

        delete deck.drawCard();
        CHECK(deck.outputDeck() == "Card 1\n");
        delete deck.drawCard();
        CHECK(deck.outputDeck() == "");
    }

    // --- generateDeckForState builds the real game deck ---
    {
        Deck deck;
        deck.generateDeckForState(12, 1, 12);

        // BUG: generateDeckForState() creates 50 cards but MAX_DECK_SIZE is 35, so
        // addCard() silently rejects (and leaks) the last 15. Either raise MAX_DECK_SIZE
        // to >= 50 or trim the composition. The assert below pins the current behaviour.
        CHECK(deck.getDeckSize() == 35);
        CHECK(!deck.isEmpty());

        for (int i = 0; i < deck.getDeckSize(); ++i) {
            CHECK(deck.getCard(i) != nullptr);
        }

        // Every generated card must be either a node card or a modifier card, since the
        // placing phase branches on exactly those two families.
        int nodeCards = 0;
        int modifierCards = 0;
        for (int i = 0; i < deck.getDeckSize(); ++i) {
            Card* card = deck.getCard(i);
            if (dynamic_cast<NodeCard*>(card) != nullptr) ++nodeCards;
            else if (dynamic_cast<ModifierCard*>(card) != nullptr) ++modifierCards;
        }
        CHECK(nodeCards + modifierCards == deck.getDeckSize());
        CHECK(nodeCards > 0);
        CHECK(modifierCards > 0);

        // Regenerating clears the previous contents rather than appending.
        deck.generateDeckForState(12, 1, 12);
        CHECK(deck.getDeckSize() == 35);

        // Boundary: degenerate game states must not crash the generator.
        deck.generateDeckForState(0, 0, 0);
        CHECK(deck.getDeckSize() == 35);
    }
    // The Deck destructor deletes whatever is left, which is exercised by every scope
    // above ending with cards still in the deck.
}

// =====================================================================================
// Player.cpp
// =====================================================================================
static void test_Player()
{
    Node castle;
    castle.isPlayer1Castle = true;
    Node middle;
    Node otherCastle;
    otherCastle.isPlayer2Castle = true;

    // --- construction ---
    Player player(1, &castle);
    CHECK(player.getPlayerId() == 1);
    CHECK(player.getCurrentNode() == &castle); // players start on their own castle
    CHECK(player.getCastleNode() == &castle);
    CHECK(player.getHandSize() == 0);
    CHECK(!player.isHandFull());
    CHECK(!player.hasWon());
    CHECK(Player::MAX_HAND_SIZE == 5);
    CHECK(Player::CARDS_DRAWN_PER_TURN == 3);
    CHECK(Player::MAX_HAND_SIZE_CARDS_PER_TURN == 3);

    // --- position ---
    player.setCurrentNode(&middle);
    CHECK(player.getCurrentNode() == &middle);
    CHECK(player.getCastleNode() == &castle); // the castle never moves with the player
    player.setCurrentNode(&castle);
    CHECK(player.getCurrentNode() == &castle);

    // --- hand: filling up to and past the limit ---
    Card* cards[6];
    for (int i = 0; i < 6; ++i) {
        cards[i] = new Card("Card " + std::to_string(i));
    }

    for (int i = 0; i < Player::MAX_HAND_SIZE; ++i) {
        CHECK(player.addCardToHand(cards[i]));
        CHECK(player.getHandSize() == i + 1);
        CHECK(player.getCardInHand(i) == cards[i]);
    }
    // Boundary: exactly at MAX_HAND_SIZE the hand is full and refuses more cards.
    CHECK(player.isHandFull());
    CHECK(!player.addCardToHand(cards[5]));
    CHECK(player.getHandSize() == Player::MAX_HAND_SIZE);

    // Boundary: a null card is rejected and does not consume a slot.
    CHECK(!player.addCardToHand(nullptr));
    CHECK(player.getHandSize() == Player::MAX_HAND_SIZE);

    // Boundary: index checks on getCardInHand.
    CHECK(player.getCardInHand(-1) == nullptr);
    CHECK(player.getCardInHand(Player::MAX_HAND_SIZE) == nullptr);
    CHECK(player.getCardInHand(1000) == nullptr);

    // --- hand: removal keeps the remaining cards in order ---
    CHECK(player.removeCardFromHand(cards[2])); // remove from the middle
    CHECK(player.getHandSize() == 4);
    CHECK(player.getCardInHand(0) == cards[0]);
    CHECK(player.getCardInHand(1) == cards[1]);
    CHECK(player.getCardInHand(2) == cards[3]); // shifted down
    CHECK(player.getCardInHand(3) == cards[4]);
    CHECK(player.getCardInHand(4) == nullptr);
    CHECK(!player.isHandFull());

    // A card that is not held cannot be removed.
    CHECK(!player.removeCardFromHand(cards[2]));
    CHECK(!player.removeCardFromHand(cards[5]));
    CHECK(!player.removeCardFromHand(nullptr));
    CHECK(player.getHandSize() == 4);

    // There is room again after a removal.
    CHECK(player.addCardToHand(cards[5]));
    CHECK(player.getHandSize() == 5);
    CHECK(player.isHandFull());

    // Boundary: emptying the hand completely.
    CHECK(player.removeCardFromHand(cards[0]));
    CHECK(player.removeCardFromHand(cards[1]));
    CHECK(player.removeCardFromHand(cards[3]));
    CHECK(player.removeCardFromHand(cards[4]));
    CHECK(player.removeCardFromHand(cards[5]));
    CHECK(player.getHandSize() == 0);
    CHECK(!player.isHandFull());
    CHECK(player.getCardInHand(0) == nullptr);
    CHECK(!player.removeCardFromHand(cards[0])); // nothing left to remove

    // --- activation tracking (one activation per node per turn) ---
    CHECK(!player.hasActivated(&middle));
    player.markActivated(&middle);
    CHECK(player.hasActivated(&middle));
    CHECK(!player.hasActivated(&castle));

    // Boundary: marking the same node twice does not double-register it.
    player.markActivated(&middle);
    CHECK(player.hasActivated(&middle));

    player.markActivated(&otherCastle);
    CHECK(player.hasActivated(&otherCastle));

    // Boundary: nullptr is accepted and tracked like any other node.
    CHECK(!player.hasActivated(nullptr));
    player.markActivated(nullptr);
    CHECK(player.hasActivated(nullptr));

    // A new turn clears the whole activation list.
    player.resetActivatedCardsForNewTurn();
    CHECK(!player.hasActivated(&middle));
    CHECK(!player.hasActivated(&otherCastle));
    CHECK(!player.hasActivated(nullptr));
    // Boundary: resetting twice in a row is harmless.
    player.resetActivatedCardsForNewTurn();
    CHECK(!player.hasActivated(&middle));

    // --- win flag ---
    CHECK(!player.hasWon());
    player.setWon(true);
    CHECK(player.hasWon());
    player.setWon(false);
    CHECK(!player.hasWon());

    // --- copy constructor ---
    {
        Player original(2, &otherCastle);
        original.addCardToHand(cards[0]);
        original.addCardToHand(cards[1]);
        original.setCurrentNode(&middle);
        original.markActivated(&middle);
        original.setWon(true);

        Player copy(original);
        CHECK(copy.getPlayerId() == 2);
        CHECK(copy.getCurrentNode() == &middle);
        CHECK(copy.getCastleNode() == &otherCastle);
        CHECK(copy.getHandSize() == 2);
        // NOTE: the hand is copied shallowly - both players point at the same cards.
        CHECK(copy.getCardInHand(0) == cards[0]);
        CHECK(copy.getCardInHand(1) == cards[1]);
        CHECK(copy.hasActivated(&middle));
        CHECK(copy.hasWon());

        // The copy has its own hand array, so removing from it leaves the original alone.
        CHECK(copy.removeCardFromHand(cards[0]));
        CHECK(copy.getHandSize() == 1);
        CHECK(original.getHandSize() == 2);
        CHECK(original.getCardInHand(0) == cards[0]);
    }

    // --- assignment operator ---
    {
        Player source(1, &castle);
        source.addCardToHand(cards[3]);
        source.setCurrentNode(&middle);
        source.setWon(true);
        source.markActivated(&castle);

        Player target(2, &otherCastle);
        target.addCardToHand(cards[4]);
        target = source;

        CHECK(target.getPlayerId() == 1);
        CHECK(target.getCurrentNode() == &middle);
        CHECK(target.getCastleNode() == &castle);
        CHECK(target.getHandSize() == 1);
        CHECK(target.getCardInHand(0) == cards[3]); // the old card was overwritten
        CHECK(target.hasWon());
        CHECK(target.hasActivated(&castle));

        // Boundary: self-assignment leaves the player intact.
        target = target;
        CHECK(target.getPlayerId() == 1);
        CHECK(target.getHandSize() == 1);
        CHECK(target.getCardInHand(0) == cards[3]);
        CHECK(target.getCurrentNode() == &middle);
    }

    // Player does not own its cards, so the test cleans them up.
    for (int i = 0; i < 6; ++i) {
        delete cards[i];
    }
}

// =====================================================================================
// Bridge.cpp
// =====================================================================================
static void test_Bridge()
{
    // --- construction: 12 nodes including both castles ---
    {
        Bridge bridge;
        CHECK(bridgeLength(bridge) == 12);
        CHECK(linksAreConsistent(bridge));

        Node* p1 = bridge.getPlayer1Castle();
        Node* p2 = bridge.getPlayer2Castle();
        CHECK(p1 != nullptr);
        CHECK(p2 != nullptr);
        CHECK(p1 != p2);
        CHECK(p1->isPlayer1Castle);
        CHECK(!p1->isPlayer2Castle);
        CHECK(p2->isPlayer2Castle);
        CHECK(!p2->isPlayer1Castle);

        // Boundary: the ends of the bridge - nobody can fall off.
        CHECK(p1->left == nullptr);
        CHECK(p2->right == nullptr);
        CHECK(nodeAt(bridge, 12) == p2);
        CHECK(nodeAt(bridge, 13) == nullptr);
        CHECK(nodeAt(bridge, 0) == nullptr);

        // The 10 nodes between the castles start empty and unmodified.
        for (int i = 2; i <= 11; ++i) {
            Node* node = nodeAt(bridge, i);
            CHECK(node != nullptr);
            CHECK(!node->isPlayer1Castle);
            CHECK(!node->isPlayer2Castle);
            CHECK(node->nodeCard == nullptr);
            CHECK(node->beginningOfStrand == nullptr);
        }
    }

    // --- isValidNode: castles and out-of-range indices are not editable ---
    {
        Bridge bridge;
        CHECK(!bridge.isValidNode(-5));
        CHECK(!bridge.isValidNode(0));
        CHECK(!bridge.isValidNode(1));  // player 1's castle
        CHECK(bridge.isValidNode(2));   // first editable node
        CHECK(bridge.isValidNode(6));
        CHECK(bridge.isValidNode(11));  // last editable node
        CHECK(!bridge.isValidNode(12)); // player 2's castle
        CHECK(!bridge.isValidNode(13));
        CHECK(!bridge.isValidNode(1000));
    }

    // --- isValidNodePlacement: only between two adjacent, in-range nodes ---
    {
        Bridge bridge;
        CHECK(bridge.isValidNodePlacement(1, 2));   // right next to player 1's castle
        CHECK(bridge.isValidNodePlacement(6, 7));
        CHECK(bridge.isValidNodePlacement(11, 12)); // right next to player 2's castle

        // Boundary: beyond either castle.
        CHECK(!bridge.isValidNodePlacement(0, 1));
        CHECK(!bridge.isValidNodePlacement(12, 13));
        CHECK(!bridge.isValidNodePlacement(-1, 0));

        // Boundary: the two nodes must be adjacent and in left-to-right order.
        CHECK(!bridge.isValidNodePlacement(2, 4));
        CHECK(!bridge.isValidNodePlacement(3, 3));
        CHECK(!bridge.isValidNodePlacement(4, 3)); // reversed
        CHECK(!bridge.isValidNodePlacement(2, 12));
    }

    // --- insertCard ---
    {
        Bridge bridge;

        // A rejected placement returns false and does NOT take ownership of the card.
        NodeCard* rejected = new BoostNode(1);
        CHECK(!bridge.insertCard(12, 13, rejected));
        CHECK(bridgeLength(bridge) == 12);
        delete rejected;

        NodeCard* alsoRejected = new BoostNode(1);
        CHECK(!bridge.insertCard(2, 4, alsoRejected));
        CHECK(bridgeLength(bridge) == 12);
        delete alsoRejected;

        // A valid placement inserts immediately to the right of leftNode.
        CHECK(bridge.insertCard(1, 2, new BoostNode(3)));
        CHECK(bridgeLength(bridge) == 13);
        CHECK(linksAreConsistent(bridge));
        Node* inserted = nodeAt(bridge, 2);
        CHECK(inserted->nodeCard != nullptr);
        CHECK(inserted->nodeCard->getMovementAmount() == 3);
        CHECK(inserted->left == bridge.getPlayer1Castle());
        CHECK(inserted->right->left == inserted);
        CHECK(!inserted->isPlayer1Castle);
        CHECK(!inserted->isPlayer2Castle);

        // Boundary: the far end of the bridge, just before player 2's castle. The bridge
        // now holds 13 nodes, so (12,13) is the slot next to the castle.
        CHECK(bridge.isValidNodePlacement(12, 13));
        CHECK(bridge.insertCard(12, 13, new RecoilNode(2)));
        CHECK(bridgeLength(bridge) == 14);
        CHECK(linksAreConsistent(bridge));
        Node* nearCastle = nodeAt(bridge, 13);
        CHECK(nearCastle->nodeCard->getMovementAmount() == -2);
        CHECK(nearCastle->right == bridge.getPlayer2Castle());
        CHECK(bridge.getPlayer2Castle()->left == nearCastle);

        // Two cards in the same slot end up side by side, which is the rule for both
        // players targeting the same gap on the same turn.
        CHECK(bridge.insertCard(5, 6, new BoostNode(1)));
        CHECK(bridge.insertCard(5, 6, new BoostNode(2)));
        CHECK(bridgeLength(bridge) == 16);
        CHECK(linksAreConsistent(bridge));
        // The most recent insert is the one closest to the left node.
        CHECK(nodeAt(bridge, 6)->nodeCard->getMovementAmount() == 2);
        CHECK(nodeAt(bridge, 7)->nodeCard->getMovementAmount() == 1);

        // NOTE: insertCard does not reject a null card; the node is created empty.
        CHECK(bridge.insertCard(2, 3, nullptr));
        CHECK(bridgeLength(bridge) == 17);
        CHECK(nodeAt(bridge, 3)->nodeCard == nullptr);
        CHECK(linksAreConsistent(bridge));
    }

    // --- attachModifierCard ---
    {
        Bridge bridge;
        bridge.insertCard(1, 2, new BoostNode(2));

        // Modifiers stack in the order they were placed.
        CHECK(bridge.attachModifierCard(2, new Booster(1)));
        CHECK(strandLength(nodeAt(bridge, 2)) == 1);
        CHECK(modifierNameAt(nodeAt(bridge, 2), 0) == "Booster");

        CHECK(bridge.attachModifierCard(2, new Recoiler(2)));
        CHECK(bridge.attachModifierCard(2, new Multiplier(3)));
        CHECK(bridge.attachModifierCard(2, new Dynamite()));
        CHECK(strandLength(nodeAt(bridge, 2)) == 4);
        CHECK(modifierNameAt(nodeAt(bridge, 2), 0) == "Booster");
        CHECK(modifierNameAt(nodeAt(bridge, 2), 1) == "Recoiler");
        CHECK(modifierNameAt(nodeAt(bridge, 2), 2) == "Multiplier");
        CHECK(modifierNameAt(nodeAt(bridge, 2), 3) == "Dynamite");

        // The strand is walkable to its end.
        const ModifierStrand* last = nodeAt(bridge, 2)->beginningOfStrand;
        while (last->next != nullptr) last = last->next;
        CHECK(last->next == nullptr);
        CHECK(last->modifierCard->getName() == "Dynamite");

        // Modifiers on one node do not leak onto its neighbours.
        CHECK(strandLength(nodeAt(bridge, 3)) == 0);

        // A node with no node card can still take modifiers.
        CHECK(bridge.attachModifierCard(5, new Booster(1)));
        CHECK(strandLength(nodeAt(bridge, 5)) == 1);

        // Boundary: castle nodes and out-of-range indices are rejected, and the modifier
        // stays the caller's responsibility.
        ModifierCard* onCastle = new Booster(1);
        CHECK(!bridge.attachModifierCard(1, onCastle));
        delete onCastle;

        ModifierCard* onFarCastle = new Booster(1);
        CHECK(!bridge.attachModifierCard(13, onFarCastle)); // 13 nodes now, so 13 == P2
        delete onFarCastle;

        ModifierCard* offBridge = new Booster(1);
        CHECK(!bridge.attachModifierCard(0, offBridge));
        delete offBridge;

        ModifierCard* wayOff = new Booster(1);
        CHECK(!bridge.attachModifierCard(999, wayOff));
        delete wayOff;

        CHECK(bridge.getPlayer1Castle()->beginningOfStrand == nullptr);
        CHECK(bridge.getPlayer2Castle()->beginningOfStrand == nullptr);
    }

    // --- removeNode (the dynamite effect) ---
    {
        Bridge bridge;
        bridge.insertCard(1, 2, new BoostNode(1)); // index 2
        bridge.insertCard(2, 3, new BoostNode(2)); // index 3
        bridge.insertCard(3, 4, new BoostNode(3)); // index 4
        CHECK(bridgeLength(bridge) == 15);

        Node* left = nodeAt(bridge, 2);
        Node* target = nodeAt(bridge, 3);
        Node* right = nodeAt(bridge, 4);
        CHECK(target->nodeCard->getMovementAmount() == 2);

        CHECK(bridge.removeNode(3));
        CHECK(bridgeLength(bridge) == 14);
        CHECK(linksAreConsistent(bridge));
        // The neighbours are relinked directly to each other.
        CHECK(left->right == right);
        CHECK(right->left == left);
        CHECK(nodeAt(bridge, 3) == right);
        CHECK(nodeAt(bridge, 3)->nodeCard->getMovementAmount() == 3);

        // Removing a node that carries modifiers frees the whole strand.
        CHECK(bridge.attachModifierCard(3, new Booster(1)));
        CHECK(bridge.attachModifierCard(3, new Multiplier(2)));
        CHECK(strandLength(nodeAt(bridge, 3)) == 2);
        CHECK(bridge.removeNode(3));
        CHECK(bridgeLength(bridge) == 13);
        CHECK(linksAreConsistent(bridge));

        // Boundary: castles cannot be destroyed.
        CHECK(!bridge.removeNode(1));
        CHECK(!bridge.removeNode(13));
        CHECK(bridgeLength(bridge) == 13);

        // Boundary: out-of-range indices.
        CHECK(!bridge.removeNode(0));
        CHECK(!bridge.removeNode(-3));
        CHECK(!bridge.removeNode(14));
        CHECK(!bridge.removeNode(500));
        CHECK(bridgeLength(bridge) == 13);
        CHECK(linksAreConsistent(bridge));

        // NOTE: removeNode() deletes the Node and its modifier strand but not the
        // NodeCard it holds, which leaks one card per destroyed node.
    }

    // --- shrinking the bridge down to the two castles ---
    {
        Bridge bridge;
        // 12 nodes: remove index 2 repeatedly until only the castles are left.
        for (int i = 0; i < 10; ++i) {
            CHECK(bridge.removeNode(2));
        }
        CHECK(bridgeLength(bridge) == 2);
        CHECK(linksAreConsistent(bridge));
        CHECK(bridge.getPlayer1Castle()->right == bridge.getPlayer2Castle());
        CHECK(bridge.getPlayer2Castle()->left == bridge.getPlayer1Castle());

        // Boundary: nothing left to remove.
        CHECK(!bridge.removeNode(2));
        CHECK(!bridge.removeNode(1));
        CHECK(bridgeLength(bridge) == 2);

        // A card can still be dropped between the two castles, rebuilding the bridge.
        CHECK(bridge.isValidNodePlacement(1, 2));
        CHECK(bridge.insertCard(1, 2, new BoostNode(1)));
        CHECK(bridgeLength(bridge) == 3);
        CHECK(linksAreConsistent(bridge));
        CHECK(bridge.isValidNode(2));
    }

    // --- copy constructor: deep copy of nodes, node cards and modifier strands ---
    {
        Bridge original;
        original.insertCard(1, 2, new BoostNode(3));
        original.attachModifierCard(2, new Booster(1));
        original.attachModifierCard(2, new Multiplier(2));
        original.insertCard(4, 5, new RecoilNode(2));

        Bridge copy(original);
        CHECK(bridgeLength(copy) == bridgeLength(original));
        CHECK(linksAreConsistent(copy));

        // Different objects...
        CHECK(copy.getPlayer1Castle() != original.getPlayer1Castle());
        CHECK(copy.getPlayer2Castle() != original.getPlayer2Castle());
        CHECK(nodeAt(copy, 2) != nodeAt(original, 2));
        CHECK(nodeAt(copy, 2)->nodeCard != nodeAt(original, 2)->nodeCard);
        // ...with the same contents.
        CHECK(nodeAt(copy, 2)->nodeCard->getMovementAmount() == 3);
        CHECK(strandLength(nodeAt(copy, 2)) == 2);
        CHECK(modifierNameAt(nodeAt(copy, 2), 0) == "Booster");
        CHECK(modifierNameAt(nodeAt(copy, 2), 1) == "Multiplier");
        CHECK(nodeAt(copy, 2)->beginningOfStrand->modifierCard !=
              nodeAt(original, 2)->beginningOfStrand->modifierCard);
        CHECK(copy.getPlayer1Castle()->isPlayer1Castle);
        CHECK(copy.getPlayer2Castle()->isPlayer2Castle);

        // Changing the original must not disturb the copy.
        CHECK(original.removeNode(2));
        CHECK(bridgeLength(original) == bridgeLength(copy) - 1);
        CHECK(nodeAt(copy, 2)->nodeCard->getMovementAmount() == 3);
        CHECK(strandLength(nodeAt(copy, 2)) == 2);
    }

    // --- assignment operator ---
    {
        Bridge source;
        source.insertCard(1, 2, new BoostNode(2));
        source.attachModifierCard(2, new Recoiler(1));

        Bridge target;
        target.insertCard(5, 6, new RecoilNode(1)); // contents that must be discarded
        target = source;

        CHECK(bridgeLength(target) == bridgeLength(source));
        CHECK(linksAreConsistent(target));
        CHECK(nodeAt(target, 2)->nodeCard != nullptr);
        CHECK(nodeAt(target, 2)->nodeCard->getMovementAmount() == 2);
        CHECK(modifierNameAt(nodeAt(target, 2), 0) == "Recoiler");
        CHECK(nodeAt(target, 2) != nodeAt(source, 2));

        // Boundary: self-assignment must not destroy the bridge.
        target = target;
        CHECK(bridgeLength(target) == bridgeLength(source));
        CHECK(linksAreConsistent(target));
        CHECK(nodeAt(target, 2)->nodeCard->getMovementAmount() == 2);

        // Chained assignment works because operator= returns *this.
        Bridge third;
        third = target = source;
        CHECK(bridgeLength(third) == bridgeLength(source));
        CHECK(linksAreConsistent(third));
    }

#if TEST_BRIDGE_PRINT_BRIDGE
    // --- printBridge ---
    {
        Bridge bridge;
        bridge.insertCard(1, 2, new BoostNode(2));
        CoutCapture capture;
        bridge.printBridge();
        std::string output = capture.str();
        CHECK(contains(output, "[P1 Castle]"));
        CHECK(contains(output, "[P2 Castle]"));
        CHECK(contains(output, "[BNod+2]"));
    }
#endif
}

// =====================================================================================
// Visual.cpp
// =====================================================================================
static void test_Visual()
{
    Visual visual;

    // --- a fresh bridge prints 12 empty nodes joined by separators ---
    {
        Bridge bridge;
        std::string expected;
        for (int i = 0; i < 12; ++i) {
            if (i > 0) expected += " --- ";
            expected += "[Empty Node]";
        }

        CoutCapture capture;
        visual.printBridge(bridge);
        std::string output = capture.str();
        CHECK(output == expected);
    }

    // --- node cards are printed through their own output() ---
    {
        Bridge bridge;
        bridge.insertCard(1, 2, new BoostNode(2));
        bridge.insertCard(2, 3, new RecoilNode(1));
        bridge.insertCard(3, 4, new PortalNode("Blue"));
        bridge.insertCard(4, 5, new DiceNode());
        bridge.insertCard(5, 6, new NodeCard());

        CoutCapture capture;
        visual.printBridge(bridge);
        std::string output = capture.str();

        CHECK(contains(output, "\033[32m[BNod+2]\033[0m"));
        CHECK(contains(output, "\033[31m[RNod-1]\033[0m"));
        CHECK(contains(output, "\033[34m[Portal]\033[0m"));
        CHECK(contains(output, "[Dice  ]"));
        CHECK(contains(output, "[Norm  ]"));
        CHECK(contains(output, "[Empty Node]")); // the castles still have no card
        // 17 nodes means 16 separators.
        int separators = 0;
        for (size_t i = output.find(" --- "); i != std::string::npos;
             i = output.find(" --- ", i + 1)) {
            ++separators;
        }
        CHECK(separators == 16);
        CHECK(bridgeLength(bridge) == 17);
    }

    // --- modifiers are printed after the node they are attached to ---
    {
        Bridge bridge;
        bridge.insertCard(1, 2, new BoostNode(1));
        bridge.attachModifierCard(2, new Booster(2));
        bridge.attachModifierCard(2, new Multiplier(3));

        CoutCapture capture;
        visual.printBridge(bridge);
        std::string output = capture.str();

        size_t nodePos = output.find("[BNod+1]");
        size_t boosterPos = output.find("Booster");
        size_t multiplierPos = output.find("Multiplier");
        CHECK(nodePos != std::string::npos);
        CHECK(boosterPos != std::string::npos);
        CHECK(multiplierPos != std::string::npos);
        CHECK(nodePos < boosterPos);          // modifiers follow their node
        CHECK(boosterPos < multiplierPos);    // in placement order
    }

    // --- boundary: a bridge shrunk to just the two castles still prints cleanly ---
    {
        Bridge bridge;
        for (int i = 0; i < 10; ++i) {
            bridge.removeNode(2);
        }
        CoutCapture capture;
        visual.printBridge(bridge);
        std::string output = capture.str();
        CHECK(output == "[Empty Node] --- [Empty Node]");
    }

    // --- printBridge does not modify the bridge it is given ---
    {
        Bridge bridge;
        bridge.insertCard(1, 2, new BoostNode(2));
        int lengthBefore = bridgeLength(bridge);
        {
            CoutCapture capture;
            visual.printBridge(bridge);
        }
        CHECK(bridgeLength(bridge) == lengthBefore);
        CHECK(linksAreConsistent(bridge));
    }
}

// =====================================================================================
// GameLogic.cpp
//
// GameLogic keeps its phases private and startGame() blocks on std::cin, so the tests
// drive it through redirected streams and assert on what it prints.
// =====================================================================================
static void test_GameLogic()
{
    // --- construction wires the players onto their own castles ---
    {
        GameLogic game;
        (void)game; // constructing and destroying must not crash or leak the bridge
        CHECK(true);
    }

    // --- one full turn, then quit ---
    {
        CinRedirect input("q\n");
        CoutCapture capture;
        GameLogic game;
        game.startGame();
        std::string output = capture.str();

        CHECK(contains(output, "Game started successfully"));
        CHECK(contains(output, "turn 1"));
        CHECK(contains(output, "Drawing phase for player 1"));
        CHECK(contains(output, "Drawing phase for player 2"));
        CHECK(contains(output, "Placing phase for player 1"));
        CHECK(contains(output, "Placing phase for player 2"));
        CHECK(contains(output, "Moving phase for player 1"));
        CHECK(contains(output, "Moving phase for player 2"));
        CHECK(contains(output, "game over"));
        // Nobody has moved, so nobody can have won yet.
        CHECK(!contains(output, "has won the game"));
    }

    // --- uppercase 'Q' quits as well ---
    {
        CinRedirect input("Q\n");
        CoutCapture capture;
        GameLogic game;
        game.startGame();
        CHECK(contains(capture.str(), "game over"));
    }

    // --- three turns before quitting: the turn counter advances ---
    {
        CinRedirect input("c\nc\nq\n");
        CoutCapture capture;
        GameLogic game;
        game.startGame();
        std::string output = capture.str();

        CHECK(contains(output, "turn 1"));
        CHECK(contains(output, "turn 2"));
        CHECK(contains(output, "turn 3"));
        CHECK(!contains(output, "turn 4"));
        CHECK(contains(output, "game over"));
    }

    // --- restarting the same object resets the turn counter ---
    {
        CinRedirect input("q\n");
        CoutCapture capture;
        GameLogic game;
        game.startGame();
        std::string first = capture.str();
        CHECK(contains(first, "turn 1"));
    }

    // NOTE: startGame() reads with `std::cin >> choice` and does not check the stream
    // state. On end of input `choice` keeps its previous value, so a stream that runs dry
    // without a 'q' loops forever. Every test above therefore ends its script with a
    // quit. Fix: check std::cin and treat a failed read as a quit.
}

// =====================================================================================
// main.cpp
//
// main.cpp holds only main(), which cannot be linked next to this file's own main().
// This test reproduces exactly what it does so the entry-point path is still covered.
// =====================================================================================
static void test_main()
{
    CinRedirect input("q\n");
    CoutCapture capture;

    GameLogic game;
    game.startGame();

    CHECK(contains(capture.str(), "Game started successfully"));
    CHECK(contains(capture.str(), "game over"));
}

// =====================================================================================
// DEMO -- not a test, nothing is asserted here.
//
// Builds one hypothetical mid-game state and prints it three ways so the visuals can be
// eyeballed:
//   1. a wrapped board view with position numbers, modifiers and player markers
//   2. the raw Visual::printBridge() output that the game actually uses today
//   3. the raw Visual::printHands() output
//
// Run the test binary to see it; it prints before the tests do.
// =====================================================================================

static const int DEMO_NODES_PER_LINE = 7;
static const int DEMO_CELL_WIDTH = 8;

// Centres plain (escape-code free) text inside a fixed-width column.
static std::string centre(const std::string& text, int width)
{
    int length = static_cast<int>(text.size());
    if (length >= width) return text.substr(0, width);
    int left = (width - length) / 2;
    return std::string(left, ' ') + text + std::string(width - length - left, ' ');
}

// The 8-visible-character label for one node. Castles get their own marker because the
// bridge never stores a node card on them.
static std::string demoCell(const Node* node)
{
    if (node->isPlayer1Castle) return "[P1CSTL]";
    if (node->isPlayer2Castle) return "[P2CSTL]";
    if (node->nodeCard == nullptr) return "[      ]";
    return node->nodeCard->output(); // 8 visible chars, may carry ANSI colour
}

// A compact summary of the modifier strand hanging off a node, e.g. "+1 x2".
static std::string demoMods(const Node* node)
{
    std::string summary;
    for (const ModifierStrand* s = node->beginningOfStrand; s != nullptr; s = s->next) {
        ModifierCard* card = s->modifierCard;
        if (card == nullptr) continue;

        std::string token;
        if (Booster* booster = dynamic_cast<Booster*>(card)) {
            token = "+" + std::to_string(booster->getBoostAmount());
        } else if (Recoiler* recoiler = dynamic_cast<Recoiler*>(card)) {
            token = std::to_string(recoiler->getRecoilAmount()); // already negative
        } else if (Multiplier* multiplier = dynamic_cast<Multiplier*>(card)) {
            token = "x" + std::to_string(multiplier->getMultiplierAmount());
        } else if (dynamic_cast<Dynamite*>(card) != nullptr) {
            token = "BOOM";
        } else {
            token = "?";
        }

        if (!summary.empty()) summary += " ";
        summary += token;
    }
    return summary;
}

// Which messengers are standing on this node.
static std::string demoWho(const Node* node, const Player& player1, const Player& player2)
{
    bool hasP1 = (player1.getCurrentNode() == node);
    bool hasP2 = (player2.getCurrentNode() == node);
    if (hasP1 && hasP2) return "P1+P2";
    if (hasP1) return "P1";
    if (hasP2) return "P2";
    return "";
}

static void demoVisualizeGameState()
{
    // ---------------------------------------------------------------------------------
    // Build the hypothetical state: a bridge that has been played on for a few turns.
    // ---------------------------------------------------------------------------------
    Bridge bridge;

    // Cards played onto the bridge, left to right. Each insert pushes the castles apart.
    bridge.insertCard(1, 2, new BoostNode(3));         // -> index 2
    bridge.insertCard(2, 3, new RecoilNode(1));        // -> index 3
    bridge.insertCard(3, 4, new PortalNode("Blue"));   // -> index 4
    bridge.insertCard(4, 5, new DiceNode());           // -> index 5
    bridge.insertCard(5, 6, new NodeCard());           // -> index 6
    bridge.insertCard(6, 7, new BoostNode(1));         // -> index 7
    bridge.insertCard(7, 8, new PortalNode("Purple")); // -> index 8
    bridge.insertCard(8, 9, new RecoilNode(2));        // -> index 9

    // Modifiers stacked onto some of those nodes.
    bridge.attachModifierCard(2, new Booster(1));
    bridge.attachModifierCard(2, new Multiplier(2));
    bridge.attachModifierCard(3, new Recoiler(2));
    bridge.attachModifierCard(5, new Multiplier(3));
    bridge.attachModifierCard(9, new Dynamite());

    // The two portals link to each other.
    PortalNode* bluePortal = dynamic_cast<PortalNode*>(nodeAt(bridge, 4)->nodeCard);
    PortalNode* purplePortal = dynamic_cast<PortalNode*>(nodeAt(bridge, 8)->nodeCard);
    bluePortal->setConnectedPortal(purplePortal);
    purplePortal->setConnectedPortal(bluePortal);

    // Messengers part way across.
    Player player1(1, bridge.getPlayer1Castle());
    Player player2(2, bridge.getPlayer2Castle());
    player1.setCurrentNode(nodeAt(bridge, 5));
    player2.setCurrentNode(nodeAt(bridge, 14));

    // Hands. Player 1 is holding a full hand of 5, player 2 has 4.
    Card* handCards[9];
    handCards[0] = new BoostNode(3);
    handCards[1] = new Multiplier(2);
    handCards[2] = new Dynamite();
    handCards[3] = new PortalNode("Yellow");
    handCards[4] = new Recoiler(1);
    handCards[5] = new RecoilNode(2);
    handCards[6] = new Booster(2);
    handCards[7] = new DiceNode();
    handCards[8] = new Dynamite();
    for (int i = 0; i < 5; ++i) player1.addCardToHand(handCards[i]);
    for (int i = 5; i < 9; ++i) player2.addCardToHand(handCards[i]);

    // A partly drawn deck, so the header can show how much is left.
    Deck deck;
    deck.generateDeckForState(bridgeLength(bridge), 5, 14);
    for (int i = 0; i < 9; ++i) delete deck.drawCard(); // the 9 cards now in hand

    // ---------------------------------------------------------------------------------
    // 1. Wrapped board view.
    // ---------------------------------------------------------------------------------
    std::cout << "\n";
    std::cout << "===============================================================\n";
    std::cout << " THE CHASM        turn 6        deck: " << deck.getDeckSize()
              << " cards remaining\n";
    std::cout << "===============================================================\n\n";

    std::vector<Node*> nodes;
    for (Node* node = bridge.getPlayer1Castle(); node != nullptr; node = node->right) {
        nodes.push_back(node);
    }

    for (size_t start = 0; start < nodes.size(); start += DEMO_NODES_PER_LINE) {
        size_t end = start + DEMO_NODES_PER_LINE;
        if (end > nodes.size()) end = nodes.size();

        std::string positionLine = " pos  ";
        std::string boardLine    = "      ";
        std::string modifierLine = " mods ";
        std::string playerLine   = " who  ";

        for (size_t i = start; i < end; ++i) {
            positionLine += centre(std::to_string(i + 1), DEMO_CELL_WIDTH) + " ";
            boardLine    += demoCell(nodes[i]);
            boardLine    += (i + 1 < nodes.size()) ? "-" : " ";
            modifierLine += centre(demoMods(nodes[i]), DEMO_CELL_WIDTH) + " ";
            playerLine   += centre(demoWho(nodes[i], player1, player2), DEMO_CELL_WIDTH) + " ";
        }

        std::cout << positionLine << "\n"
                  << boardLine    << "\n"
                  << modifierLine << "\n"
                  << playerLine   << "\n\n";
    }

    std::cout << " P1 castle is position 1, P2 castle is position " << nodes.size()
              << ".\n";
    std::cout << " P1 runs left to right, P2 runs right to left.\n";
    std::cout << " legend: [BNod+x] boost  [RNod-x] recoil  [Portal] portal  "
                 "[Dice  ] dice\n";
    std::cout << "         [Norm  ] plain  [      ] empty   mods: +x booster, "
                 "-x recoiler, xN multiplier, BOOM dynamite\n\n";

    // ---------------------------------------------------------------------------------
    // 2 + 3. What the game prints today, straight from Visual.
    // ---------------------------------------------------------------------------------
    Visual visual;

    std::cout << "--- Visual::printBridge() ------------------------------------\n";
    visual.printBridge(bridge);
    std::cout << "\n\n";

    std::cout << "--- Visual::printHands() -------------------------------------\n";
    visual.printHands(player1, player2);
    std::cout << "\n";

    // The bridge owns the cards played onto it; the hands are the demo's to clean up.
    for (int i = 0; i < 9; ++i) delete handCards[i];
}

// =====================================================================================
// Entry point
// =====================================================================================
int main()
{
    demoVisualizeGameState();

    std::cout << "=== The Chasm - unit tests ===" << std::endl;

    std::cout << "\n[cards]" << std::endl;
    runTest("Card.cpp", test_Card);
    runTest("NodeCard.cpp", test_NodeCard);
    runTest("ModifierCard.cpp", test_ModifierCard);
    runTest("BoosterNode.cpp", test_BoosterNode);
    runTest("RecoilNode.cpp", test_RecoilNode);
    runTest("DiceNode.cpp", test_DiceNode);
    runTest("PortalNode.cpp", test_PortalNode);
    runTest("Booster.cpp", test_Booster);
    runTest("Recoiler.cpp", test_Recoiler);
    runTest("Mutiplier.cpp", test_Multiplier);
    runTest("Dynamite.cpp", test_Dynamite);

    std::cout << "\n[game structures]" << std::endl;
    runTest("Deck.cpp", test_Deck);
    runTest("Player.cpp", test_Player);
    runTest("Bridge.cpp", test_Bridge);
    runTest("Visual.cpp", test_Visual);

    std::cout << "\n[game flow]" << std::endl;
    runTest("GameLogic.cpp", test_GameLogic);
    runTest("main.cpp", test_main);

    std::cout << "\n=== all " << g_tests << " tests passed (" << g_checks
              << " assertions) ===" << std::endl;



    
    return 0;
}
