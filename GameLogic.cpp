#include "Player.hpp"
#include "Bridge.hpp"
#include "GameLogic.hpp"
#include "card/Card.hpp"
#include "card/NodeCard.hpp"
#include "card/ModifierCard.hpp"
#include "card/modifiers/Dynamite.hpp"
#include "card/nodecards/DiceNode.hpp"
#include "card/modifiers/Booster.hpp"
#include "card/modifiers/Recoiler.hpp"
#include "card/modifiers/Multiplier.hpp"
#include "card/nodecards/PortalNode.hpp"
#include "Visual.hpp"
#include <iostream>
#include <cmath>
#define HIDE_INPUT

// Initializes the bridge, both players, turn number, and game state.
GameLogic::GameLogic() : bridge(), player1(1, bridge.getPlayer1Castle()), player2(2, bridge.getPlayer2Castle()), turnNumber(0), gameOver(false) {
    
}

// No extra cleanup is needed because the objects manage their own memory.
GameLogic::~GameLogic() {
    
}

/*
Starts the game and controls the main game loop.
Each loop runs one turn, checks for a winner, and allows the user to quit.
*/
void GameLogic::startGame() {
    Visual visual;
    char choice;
    gameOver = false;
    turnNumber = 1;
    std::cout << "Game started successfully" << std::endl;

    // Creates the deck using the starting bridge state.
    deck.generateDeckForState(12, 1, 12);
    
    // Prints the bridge before the first turn begins.
    visual.printBridge(bridge, player1, player2);
    std::cout << std::endl;
    
    // Continues running turns until a player wins or the user quits.
    while(!gameOver) {
        runTurn();
        checkWinCondition();
        
        // Stops immediately if the turn resulted in the game ending.
        if (gameOver) {
            break;
        }

        // Allows the user to quit between turns.
        std::cout << "Press 'Q' to quit or any other key to continue: ";
        std::cin >> choice;
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        
        // Removes the user's input from the terminal when HIDE_INPUT is enabled.
        #ifdef HIDE_INPUT
        std::cout << "\033[A\r\033[2K";
        #endif 

        if (!gameOver && (choice == 'Q' || choice == 'q')) {
            gameOver = true;
            std::cout << "game over" << std::endl;
        }
    }
}

/*
Runs one complete turn.
Both players draw, place their cards, and then complete their movement phase.
*/
void GameLogic::runTurn() {
    Visual visual;
    
    // Prints the current turn number.
    std::cout << "------" << std::endl;
    std::cout << "Turn " << turnNumber << std::endl;
    std::cout << "------" << std::endl;

    // Both players draw their cards before placing begins.
    drawingPhase(player1);
    drawingPhase(player2);
    
    // Tracks whether each player has finished placing cards this turn.
    bool p1Finished = false;
    bool p2Finished = false;
    
    /*
    Gives both players a chance to place cards.
    A player is finished when they skip or reach the placement limit.
    */
    for (int round = 0; round < Player::MAX_HAND_SIZE_CARDS_PER_TURN; round++) {
        placementChoice p1choice{};
        placementChoice p2choice{};

        // Gets Player 1's choice if they have not already finished.
        if (!p1Finished) {
            p1choice = placingPhase(player1);
            if (!p1choice.valid) {
                p1Finished = true;
            }
        }

        // Gets Player 2's choice if they have not already finished.
        if (!p2Finished) {
            p2choice = placingPhase(player2);
            if (!p2choice.valid) {
                p2Finished = true;
            }
        }

        // Ends the placement phase once both players are finished.
        if (p1Finished && p2Finished) {
            break;
        }
        
        // Summary of what each player did each turn
        Card* p1card = dynamic_cast<Card*>(p1choice.card);
        Card* p2card = dynamic_cast<Card*>(p2choice.card);
        ModifierCard* p1modifier = dynamic_cast<ModifierCard*>(p1choice.card);
        ModifierCard* p2modifier = dynamic_cast<ModifierCard*>(p2choice.card);
        std::cout << "\033[34mP1\033[0m"; 
        if (p1card == nullptr) {
            std::cout << " skipped" << std::endl;
        } else {
            std::cout << " placed " << p1choice.card->Handoutput();
            if (p1modifier == nullptr) {
                std::cout << " between " << p1choice.leftNode << " and " << p1choice.rightNode << std::endl;
            } else {
                std::cout << " at " << p1choice.targetNode << std::endl;
            }
        }
        std::cout << "\033[31mP2\033[0m";
        if (p2card == nullptr) {
            std::cout << " skipped" << std::endl;
        } else {
            std::cout << " placed " << p2choice.card->Handoutput();
            if (p2modifier == nullptr) {
                std::cout << " between " << p2choice.leftNode << " and " << p2choice.rightNode << std::endl;
            } else {
                std::cout << " at " << p2choice.targetNode << std::endl;
            }    
        }

        // Resolves the two choices before showing the updated bridge.
        resolvePlacements(p1choice, p2choice);
        std::cout << std::endl;

        visual.printBridge(bridge, player1, player2);
        std::cout << std::endl << std::endl;
    }

    // Players complete their movement one at a time.
    movingPhase(player1);
    movingPhase(player2);

    // Prints the final bridge state after both players have moved.
    visual.printBridge(bridge, player1, player2);
    std::cout << std::endl;

    turnNumber++;
}

// Draws the player's cards for the current turn.
void GameLogic::drawingPhase(Player& player) {
    
    std::cout << "Drawing phase for player " << player.getPlayerId() << std::endl;

    // Attempts to draw the normal number of cards for the turn.
    for (int i = 0; i < Player::CARDS_DRAWN_PER_TURN; i++) {
        if (!deck.isEmpty()) {

            // Stops drawing if the player's hand has reached its maximum size.
            if (player.isHandFull()) {
                std::cout << "Player " << player.getPlayerId() << "'s hand is full. Cannot draw more cards." << std::endl;
                break;
            } else {
                Card* drawnCard = deck.drawCard();
                player.addCardToHand(drawnCard);
                std::cout << "Player " << player.getPlayerId() << " drew card: " << drawnCard->getName() << std::endl;
            } 
        } else {

            // Gives a normal node card when the deck has run out of cards.
            NodeCard* drawnCard = new NodeCard();
            player.addCardToHand(drawnCard);
            std::cout << "Deck is empty. Drew node cards." << std::endl;
            break;
        }
    }
    std::cout << "\n";
}

/*
Gets one placement choice from a player.
The player may choose a card and location or enter -1 to stop placing.
*/
GameLogic::placementChoice GameLogic::placingPhase(Player& player)
{
    // Creates an empty choice before collecting the player's input.
    placementChoice choice{};
    Visual visual;

    choice.player = &player;
    choice.card = nullptr;
    choice.cardIndex = -1;
    choice.leftNode = -1;
    choice.rightNode = -1;
    choice.targetNode = -1;
    choice.valid = false;

    std::cout << "Placing phase for player "
              << player.getPlayerId()
              << std::endl;

    // A player with no cards automatically finishes their placement phase.
    if (player.getHandSize() == 0)
    {
        std::cout << "Player " << player.getPlayerId() << " has no cards to place." << std::endl;

        return choice;
    }

    // Prints the current player's hand first so their choices are easier to read.
    if (player.getPlayerId() == 1) {
        visual.printHands(player1, player2);
    } else {
        visual.printHands(player2, player1);
    }

    // Keeps asking until a valid card is selected or the player skips.
    while (true)
    {
        std::cout << "Enter the index of the card you would like to place from 0 to " << player.getHandSize() - 1
        << ", or -1 to skip placing: ";

        // Clears invalid text input and asks the player again.
        if (!(std::cin >> choice.cardIndex)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Please input a number." << std::endl;
            continue;
        }
        
        // Removes the user's input from the terminal when HIDE_INPUT is enabled.
        #ifdef HIDE_INPUT
        std::cout << "\033[A\r\033[2K";
        #endif

        // Entering -1 ends this player's placement phase.
        if (choice.cardIndex == -1) {
            std::cout << "Player " << player.getPlayerId() << " is skipping placing" << std::endl;
            std::cout << "\n";
            return choice;
        }

        // Makes sure the selected index exists in the player's hand.
        if (choice.cardIndex < 0 ||
            choice.cardIndex >= player.getHandSize())
        {
            std::cout << "Invalid index. Please try again." << std::endl;
            continue;
        }

        choice.card = player.getCardInHand(choice.cardIndex);

        // Protects against an empty card pointer being selected.
        if (choice.card == nullptr)
        {
            std::cout << "No card exists at that index. Please try again." << std::endl;
            continue;
        }

        /*
        Checks whether the selected card is a NodeCard.
        Node cards require two adjacent nodes to describe where they are inserted.
        */
        NodeCard* nodeCard = dynamic_cast<NodeCard*>(choice.card);
        if (nodeCard != nullptr) {
            while(true){
                std::cout << "Enter the 2 adjacent nodes you want to place " << choice.card->Handoutput() << " between: ";

                // Clears invalid text input and asks for the two nodes again.
                if (!(std::cin >> choice.leftNode >> choice.rightNode)) {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << "Please input a number." << std::endl;
                    continue;
                }

                // Removes the user's input from the terminal when HIDE_INPUT is enabled.
                #ifdef HIDE_INPUT
                std::cout << "\033[A\r\033[2K";
                #endif

                // Checks whether the two selected nodes are valid neighbours.
                if (bridge.isValidNodePlacement(choice.leftNode, choice.rightNode)) {
                    PortalNode* portal = dynamic_cast<PortalNode*>(nodeCard);

                    /*
                    Portal cards have the extra restriction that they must be placed
                    within five nodes of the player placing them.
                    */
                    if (portal != nullptr) {
                        int playerIndex = getNodeIndex(player.getCurrentNode());
                        int portalIndex = choice.rightNode;

                        if (playerIndex == -1 || std::abs(playerIndex - portalIndex) > 5) {
                            std::cout << "Portal must be placed within 5 nodes of the player's current position." << std::endl;

                            continue;
                        }
                    }
                    break;
                }

                std::cout <<"those nodes are not adjacent." << std::endl;
                
            }
        } else {

            // Modifier cards are attached directly to one existing bridge node.
            ModifierCard* modifierCard = dynamic_cast<ModifierCard*>(choice.card);
            if (modifierCard != nullptr) {
                while (true) {
                    std::cout << "Enter the node you want to place " << choice.card->Handoutput() << " on: ";

                    // Clears invalid text input and asks for the node again.
                    if (!(std::cin >> choice.targetNode)) {
                        std::cin.clear();
                        std::cin.ignore(10000, '\n');
                        std::cout << "Please input a number." << std::endl;
                        continue;
                    }

                    // Removes the user's input from the terminal when HIDE_INPUT is enabled.
                    #ifdef HIDE_INPUT
                    std::cout << "\033[A\r\033[2K";
                    #endif

                    // Castle nodes and nodes outside the bridge cannot be modified.
                    if (bridge.isValidNode(choice.targetNode)) {
                        Dynamite* dynamite = dynamic_cast<Dynamite*>(modifierCard);
                        
                        if (dynamite != nullptr) {
                            Node* targetNode = bridge.getPlayer1Castle();

                            for (int i = 1; i < choice.targetNode; i++) {
                                targetNode = targetNode->right;
                            }

                            if (targetNode == player1.getCurrentNode() || targetNode == player2.getCurrentNode()) {
                                std::cout << "Cannot remove node with player on it, choose another node: " << std::endl;
                                continue;
                            }
                        }
                        
                        break;
                    }
                    std::cout << "Not valid, please try again." << std::endl;
                }
            }
        }

        // Reaching this point means the card and its location are valid.
        choice.valid = true;

        // std::cout << "Player "
        //           << player.getPlayerId()
        //           << " selected: "
        //           << choice.card->getName()
        //           << std::endl;

        break;
    }
    std::cout << "\n";

    return choice;
}

/*
Searches the bridge for an available matching portal.
If one is found, both portals store a pointer to each other.
*/
void GameLogic::linkPortal(PortalNode* newPortal) {
    // A null portal cannot be linked.
    if (newPortal == nullptr) {
        return;
    }

    // Searches through the bridge starting from Player 1's castle.
    Node* currentNode = bridge.getPlayer1Castle();
    while(currentNode != nullptr) {
        PortalNode* existingPortal = dynamic_cast<PortalNode*>(currentNode->nodeCard);
        
        /*
        A portal can link if it is a different portal, is currently unlinked,
        and has the same colour as the newly placed portal.
        */
        bool canLink = existingPortal != nullptr && existingPortal != newPortal && existingPortal->getConnectedPortal() == nullptr
            && existingPortal->getColour() == newPortal->getColour();

        if (canLink) {
            newPortal->setConnectedPortal(existingPortal);
            existingPortal->setConnectedPortal(newPortal);

            std::cout << "Linked two " << newPortal->getColour() << " portals." << std::endl;

            return;
        }

        currentNode = currentNode->right;
    }
}

/*
Applies one valid placement choice to the bridge.
Different card types are handled separately because their placement effects are different.
*/
bool GameLogic::applyPlacement(placementChoice& choice) {
    // Stops immediately if the placement choice does not contain the required information.
    if (!choice.valid || choice.card == nullptr || choice.player == nullptr) {
        return false;
    }

    // Dynamite removes an existing bridge node instead of attaching to it.
    Dynamite* dynamite = dynamic_cast<Dynamite*>(choice.card);
    if (dynamite != nullptr) {

        // Finds the actual node pointer using the chosen bridge index.
        Node* targetNode = bridge.getPlayer1Castle();
        for (int i = 1; i < choice.targetNode; i++) {
            targetNode = targetNode->right;
        }

        // Dynamite cannot destroy a node that currently contains either player.
        if (targetNode == player1.getCurrentNode() || targetNode == player2.getCurrentNode()) {
            std::cout << "Cannot remove node with player on it." << std::endl;

            return false;
        }

        // Attempts to remove the selected node from the bridge.
        bool succeeded = bridge.removeNode(choice.targetNode);
        if (!succeeded) {
            std::cout << "Failed to remove node at index " << choice.targetNode << std::endl;
            return false;
        }

        // Removes and deletes the Dynamite card after it is successfully used.
        choice.player->removeCardFromHand(choice.card);
        delete dynamite;
        choice.card = nullptr;


        return true;
    }

    // Node cards are inserted between the two selected adjacent nodes.
    NodeCard* nodeCard = dynamic_cast<NodeCard*>(choice.card);
    if (nodeCard != nullptr) {
        bool succeeded = bridge.insertCard(choice.leftNode, choice.rightNode, nodeCard);
        if (!succeeded) {
            return false;
        }

        // Newly inserted portal cards attempt to connect to a matching portal.
        PortalNode* newPortal = dynamic_cast<PortalNode*>(nodeCard);
        if (newPortal != nullptr) {
            linkPortal(newPortal);
        }

        // The successfully placed card is removed from the player's hand.
        choice.player->removeCardFromHand(choice.card);
        choice.card = nullptr;

        return true;
    }

    // Modifier cards are attached to the selected existing node.
    ModifierCard* modifierCard = dynamic_cast<ModifierCard*>(choice.card);
    if (modifierCard != nullptr) {
        bool succeeded = bridge.attachModifierCard(choice.targetNode, modifierCard);
        if (!succeeded) {
            return false;
        }

        // The successfully placed modifier is removed from the player's hand.
        choice.player->removeCardFromHand(choice.card);
        choice.card = nullptr;

        return true;
    }

    // Reaching this point means the card type could not be placed.
    return false;
}

/*
Resolves both players' placement choices in an order that prevents
one placement from changing the intended location of the other.
*/
void GameLogic::resolvePlacements(placementChoice& p1choice, placementChoice& p2choice) {
    Visual visual;

    // First checks whether both players selected modifiers for the same node.
    ModifierCard* p1Modifier = dynamic_cast<ModifierCard*>(p1choice.card);
    ModifierCard* p2Modifier = dynamic_cast<ModifierCard*>(p2choice.card);

    if (p1Modifier != nullptr && p2Modifier != nullptr && p1choice.targetNode == p2choice.targetNode) {

        /*
        If both players use Dynamite on the same node, the node is only removed once.
        Both Dynamite cards are still consumed when the removal succeeds.
        */
        Dynamite* p1Dynamite = dynamic_cast<Dynamite*>(p1choice.card);
        Dynamite* p2Dynamite = dynamic_cast<Dynamite*>(p2choice.card);
        if (p1Dynamite != nullptr && p2Dynamite != nullptr) {
            bool succeeded = applyPlacement(p1choice);

            if (succeeded) {
                p2choice.player->removeCardFromHand(p2choice.card);
                delete p2Dynamite;
                p2choice.card = nullptr;
            }

            return;
        }

        /*
        When a Multiplier and another modifier target the same node,
        the Multiplier is attached first.
        */
        Multiplier* p1Multiplier = dynamic_cast<Multiplier*>(p1choice.card);
        Multiplier* p2Multiplier = dynamic_cast<Multiplier*>(p2choice.card);
        if (p1Multiplier != nullptr) {
            applyPlacement(p1choice);
            applyPlacement(p2choice);
            return;
        } else if (p2Multiplier != nullptr) {
            applyPlacement(p2choice);
            applyPlacement(p1choice);
            return;
        }

    }
    
    // Checks whether both players selected node cards.
    NodeCard* p1NodeCard = dynamic_cast<NodeCard*>(p1choice.card);
    NodeCard* p2NodeCard = dynamic_cast<NodeCard*>(p2choice.card);

    if (p1NodeCard != nullptr && p2NodeCard != nullptr) {

        // Checks whether both node cards were selected for the exact same gap.
        bool sameGap = (p1choice.leftNode == p2choice.leftNode && p1choice.rightNode == p2choice.rightNode);
        
        /*
        When both cards use the same gap, Player 2's node is inserted first.
        Player 1's node is then inserted using the original gap values.
        */
        if (sameGap) {
            applyPlacement(p2choice);
            applyPlacement(p1choice);
            return;
        }

        /*
        When the cards use different gaps, the card farther to the right is placed first.
        This prevents the first inserted node from shifting the second card's node indexes.
        */
        if (p1choice.leftNode > p2choice.leftNode) {
            applyPlacement(p1choice);
            applyPlacement(p2choice);
        } else {
            applyPlacement(p2choice);
            applyPlacement(p1choice);
        }

        return;
    }

    Dynamite* p1Dynamite = dynamic_cast<Dynamite*>(p1choice.card);
    Dynamite* p2Dynamite = dynamic_cast<Dynamite*>(p2choice.card);

    /*
    If Player 1 places a node and Player 2 uses Dynamite,
    place the node first so its selected gap does not change.
    The Dynamite index is increased if the inserted node shifts its target.
    */
    if (p1NodeCard != nullptr && p2Dynamite != nullptr) {
        int insertedIndex = p1choice.rightNode;
        bool succeeded = applyPlacement(p1choice);
        if (succeeded && insertedIndex <= p2choice.targetNode) {
            p2choice.targetNode++;
        }
        applyPlacement(p2choice);
        return;
    }

    /*
    If Player 2 places a node and Player 1 uses Dynamite,
    place the node first and update the Dynamite target if needed.
    */
    if(p2NodeCard != nullptr && p1Dynamite != nullptr) {
        int insertedIndex = p2choice.rightNode;
        bool succeeded = applyPlacement(p2choice);
        if (succeeded && insertedIndex <= p1choice.targetNode) {
            p1choice.targetNode++;
        }
        applyPlacement(p1choice);
        return;
    }

    /*
    If only Player 1 selected a node card, Player 2's placement is resolved first.
    This avoids the new node changing Player 2's selected index.
    */
    if (p1NodeCard != nullptr) {
        applyPlacement(p2choice);
        applyPlacement(p1choice);
        return;
    }

    /*
    If only Player 2 selected a node card, Player 1's placement is resolved first.
    This avoids the new node changing Player 1's selected index.
    */
    if (p2NodeCard != nullptr) {
        applyPlacement(p1choice);
        applyPlacement(p2choice);
        return;
    }

    // If neither card inserts a node, the choices can be applied normally.
    applyPlacement(p1choice);
    applyPlacement(p2choice);
}

/*
Moves a player a specific number of spaces.
Positive movement goes toward the opponent's castle and negative movement goes backward.
*/
void GameLogic::movePlayer(Player& player, int amount) {

    Node* currentNode = player.getCurrentNode();

    // Stores the direction separately from the number of spaces being moved.
    bool movingForward = amount > 0;
    int steps;

    // Converts a negative movement value into a positive loop count.
    if (amount < 0) {
        steps = -amount;
    } else {
        steps = amount;
    }

    // Moves through the linked bridge one node at a time.
    for (int i = 0; i < steps; i++) {
        Node* nextNode = nullptr;

        /*
        Player 1 moves right when moving forward.
        Player 2 moves left because the players begin on opposite sides.
        */
        if (player.getPlayerId() == 1) {
            if (movingForward) {
                nextNode = currentNode->right;
            } else {
                nextNode = currentNode->left;
            }
        } else {
            if (movingForward) {
                nextNode = currentNode->left;
            } else {
                nextNode = currentNode->right;
            }
        }

        // Stops movement if there are no more nodes in that direction.
        if (nextNode == nullptr) {
            break;
        }

        currentNode = nextNode;
    }

    // Saves the player's final location after all movement is completed.
    player.setCurrentNode(currentNode);

}

// Finds the numerical bridge index belonging to a specific node pointer.
int GameLogic::getNodeIndex(const Node* targetNode) const {
    Node* currentNode = bridge.getPlayer1Castle();
    int index = 1;

    // Searches from Player 1's castle toward Player 2's castle.
    while (currentNode != nullptr) {
        if (currentNode == targetNode) {
            return index;
        }

        currentNode = currentNode->right;
        index++;
    }

    // Returns -1 when the node cannot be found on the bridge.
    return -1;
}

/*
Calculates the movement effect of a node.
The node card provides the starting movement and attached modifiers change that value.
*/
int GameLogic::calculateMovement(Node* node) {
    // A missing node has no movement effect.
    if (node == nullptr) {
        return 0;
    }

    int movement = 0;

    /*
    Dice nodes generate a new movement value whenever they activate.
    Other node cards use the movement amount stored by the card.
    */
    if (node->nodeCard != nullptr) {
        DiceNode* dicenode = dynamic_cast<DiceNode*>(node->nodeCard);
        if (dicenode != nullptr) {
        movement = dicenode->rollDice();
        } else {
        movement = node->nodeCard->getMovementAmount();
        }
    }

    // Starts at the first modifier attached to the node.
    ModifierStrand* currentModifier = node->beginningOfStrand;

    // Goes through every modifier and updates the current movement amount.
    while (currentModifier != nullptr) {

        Booster* booster = dynamic_cast<Booster*>(currentModifier->modifierCard);
        Recoiler* recoiler = dynamic_cast<Recoiler*>(currentModifier->modifierCard);
        Multiplier* multiplier = dynamic_cast<Multiplier*>(currentModifier->modifierCard);

        // Applies the effect belonging to the current modifier card.
        if (booster != nullptr) {
            movement += booster->getBoostAmount();
        } else if (recoiler != nullptr) {
            movement += recoiler->getRecoilAmount();
        } else if (multiplier != nullptr) {
            movement *= multiplier->getMultiplierAmount();
        }

        currentModifier = currentModifier->next;
    }

    return movement;
}

/*
Handles all movement for one player during their movement phase.
Movement effects continue activating until the player lands on a stopping node.
*/
void GameLogic::movingPhase(Player& player)
{
    std::cout
        << "Moving phase for player "
        << player.getPlayerId()
        << std::endl;

    // Allows nodes to activate again because this is a new turn.
    player.resetActivatedCardsForNewTurn();

    // Every movement phase begins with one normal forward step.
    movePlayer(player, 1);

    // Continues checking the node the player lands on after every movement effect.
    while (true)
    {
        Node* landedNode = player.getCurrentNode();

        // Stops if the player's current node does not exist.
        if (landedNode == nullptr)
        {
            break;
        }

        // Reaching either castle ends the player's movement chain.
        if (landedNode->isPlayer1Castle ||
            landedNode->isPlayer2Castle)
        {
            break;
        }

        /*
        A player cannot activate the same node more than once during one turn.
        This prevents movement effects from creating an endless loop.
        */
        if (player.hasActivated(landedNode))
        {
            std::cout
                << "This node already activated for Player "
                << player.getPlayerId()
                << "."
                << std::endl;

            break;
        }

        // Records the node before its effect is activated.
        player.markActivated(landedNode);

        /*
        If the player lands on a linked portal, the bridge is searched
        for the node containing the matching connected portal.
        */
        PortalNode* portal = dynamic_cast<PortalNode*>(landedNode->nodeCard);
        if (portal != nullptr && portal->getConnectedPortal() != nullptr) {
            Node* destination = bridge.getPlayer1Castle();

            while (destination != nullptr && destination -> nodeCard != portal->getConnectedPortal()) {
                destination = destination->right;
            }

            // Moves the player directly to the connected portal when it is found.
            if (destination != nullptr) {
                player.setCurrentNode(destination);

                player.markActivated(destination);

                int movement = 0;

                ModifierStrand* currentModifier = destination->beginningOfStrand;
                while (currentModifier != nullptr) {
                    Booster* booster = dynamic_cast<Booster*>(currentModifier->modifierCard);
                    Recoiler* recoiler = dynamic_cast<Recoiler*>(currentModifier->modifierCard);
                    Multiplier* multiplier = dynamic_cast<Multiplier*>(currentModifier->modifierCard);

                    if (booster != nullptr) {
                        movement += booster->getBoostAmount();
                    } else if (recoiler != nullptr) {
                        movement += recoiler->getRecoilAmount();
                    }

                    currentModifier = currentModifier->next;
                }

                // Then multipliers from first portal.
                currentModifier = landedNode->beginningOfStrand;

                while (currentModifier != nullptr) {
                    Multiplier* multiplier = dynamic_cast<Multiplier*>(currentModifier->modifierCard);

                    if (multiplier != nullptr) {
                        movement *= multiplier->getMultiplierAmount();
                    }

                    currentModifier = currentModifier->next;
                }

                // Then multipliers from second portal.
                currentModifier = destination->beginningOfStrand;

                while (currentModifier != nullptr) {
                    Multiplier* multiplier = dynamic_cast<Multiplier*>(currentModifier->modifierCard);

                    if (multiplier != nullptr) {
                        movement *= multiplier->getMultiplierAmount();
                    }

                    currentModifier = currentModifier->next;
                }

                std::cout << "Player " << player.getPlayerId() << " teleported to the connected portal." << std::endl;
                if (movement == 0) {
                    break;
                }

                movePlayer(player, movement);
                continue;
            }
        }

        // Calculates the complete movement effect of the node.
        int movement = calculateMovement(landedNode);
        
        std::string landedName = "Empty node";

        // Gets the card name so the movement can be displayed to the player.
        if (landedNode->nodeCard != nullptr) {
            landedName = landedNode->nodeCard->getName();
        }

        std::cout
            << "Player "
            << player.getPlayerId()
            << " landed on "
            << landedName
            << " and moves "
            << movement
            << "."
            << std::endl;

        // A movement value of zero ends the player's movement chain.
        if (movement == 0)
        {
            break;
        }

        // Moves the player and then checks the next node they land on.
        movePlayer(player, movement);
    }
    std::cout << "\n";

}

/*
Checks whether either player has reached the opposing castle.
If both players reach their destination during the same turn, the game is a draw.
*/
void GameLogic::checkWinCondition() {
    bool p1ReachedCastle = (player1.getCurrentNode() == bridge.getPlayer2Castle());
    bool p2ReachedCastle = (player2.getCurrentNode() == bridge.getPlayer1Castle());

    // Checks for a draw before checking either individual player.
    if (p1ReachedCastle && p2ReachedCastle) {
        std::cout << "Both players reached the opponent's castle. draw!" << std::endl;
        gameOver = true;
    } else if (p1ReachedCastle) {
        std::cout << "Player 1 reached Player 2's castle. Player 1 wins!" << std::endl;
        gameOver = true;
    } else if (p2ReachedCastle) {
        std::cout << "Player 2 reached Player 1's castle. Player 2 wins!" << std::endl;
        gameOver = true;
    }
}