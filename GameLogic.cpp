#include "Player.hpp"
#include "Bridge.hpp"
#include "GameLogic.hpp"
#include "Card.hpp"
#include "NodeCard.hpp"
#include "ModifierCard.hpp"
#include "Dynamite.hpp"
#include "DiceNode.hpp"
#include "Booster.hpp"
#include "Recoiler.hpp"
#include "Multiplier.hpp"
#include "PortalNode.hpp"
#include "Visual.hpp"
#include <iostream>
#include <cmath>

GameLogic::GameLogic() : bridge(), player1(1, bridge.getPlayer1Castle()), player2(2, bridge.getPlayer2Castle()), turnNumber(0), gameOver(false) {
    
}

GameLogic::~GameLogic() {
    
}

void GameLogic::startGame() {
    char choice;
    gameOver = false;
    turnNumber = 1;
    std::cout << "Game started successfully" << std::endl;
    deck.generateDeckForState(12, 1, 12);
    while(!gameOver) {
        runTurn();
        checkWinCondition();
        if (gameOver) {
            break;
        }
        std::cout << "Press 'Q' to quit or any other key to continue: ";
        std::cin >> choice;
        if (!gameOver && (choice == 'Q' || choice == 'q')) {
            gameOver = true;
            std::cout << "game over" << std::endl;
        }
    }
}

void GameLogic::runTurn() {
    Visual visual;

    std::cout << "turn " << turnNumber << std::endl;

    drawingPhase(player1);
    drawingPhase(player2);

    visual.printHands(player1, player2);

    bool p1Finished = false;
    bool p2Finished = false;

    for (int round = 0; round < Player::MAX_HAND_SIZE_CARDS_PER_TURN; round++) {
        placementChoice p1choice{};
        placementChoice p2choice{};

        if (!p1Finished) {
            p1choice = placingPhase(player1);
            if (!p1choice.valid) {
                p1Finished = true;
            }
        }

        if (!p2Finished) {
            p2choice = placingPhase(player2);
            if (!p2choice.valid) {
                p2Finished = true;
            }
        }

        if (p1Finished && p2Finished) {
            break;
        }

        resolvePlacements(p1choice, p2choice);
    }

    visual.printBridge(bridge);
    std::cout << std::endl;

    movingPhase(player1);
    movingPhase(player2);
    turnNumber++;
}

void GameLogic::drawingPhase(Player& player) {
    std::cout << "Drawing phase for player " << player.getPlayerId() << std::endl;
    for (int i = 0; i < Player::CARDS_DRAWN_PER_TURN; i++) {
        if (!deck.isEmpty()) {
            if (player.isHandFull()) {
                std::cout << "Player " << player.getPlayerId() << "'s hand is full. Cannot draw more cards." << std::endl;
                break;
            } else {
                Card* drawnCard = deck.drawCard();
                player.addCardToHand(drawnCard);
                std::cout << "Player " << player.getPlayerId() << " drew card: " << drawnCard->getName() << std::endl;
            } 
        } else {
            std::cout << "Deck is empty. Cannot draw more cards." << std::endl;
            break;
        }
    }
}

GameLogic::placementChoice GameLogic::placingPhase(Player& player)
{
    placementChoice choice{};

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

    if (player.getHandSize() == 0)
    {
        std::cout << "Player " << player.getPlayerId() << " has no cards to place." << std::endl;

        return choice;
    }

    for (int i = 0; i < player.getHandSize(); i++)
    {
        Card* card = player.getCardInHand(i);

        if (card != nullptr)
        {
            std::cout << i << ": " << card->getName() << std::endl;
        }
    }

    while (true)
    {
        std::cout << "Enter the index of the card you would like to place from 0 to " << player.getHandSize() - 1
        << ", or -1 to skip placing: ";

        std::cin >> choice.cardIndex;
        if (choice.cardIndex == -1) {
            std::cout << "Player " << player.getPlayerId() << "is skipping placing" << std::endl;
            return choice;
        }

        if (choice.cardIndex < 0 ||
            choice.cardIndex >= player.getHandSize())
        {
            std::cout << "Invalid index. Please try again." << std::endl;
            continue;
        }

        choice.card = player.getCardInHand(choice.cardIndex);

        if (choice.card == nullptr)
        {
            std::cout << "No card exists at that index. Please try again." << std::endl;
            continue;
        }

        NodeCard* nodeCard = dynamic_cast<NodeCard*>(choice.card);
        if (nodeCard != nullptr) {
            while(true){
                std::cout << "Enter the 2 adjacent nodes you want to place the card between: ";
                std::cin >> choice.leftNode >> choice.rightNode;
                if (bridge.isValidNodePlacement(choice.leftNode, choice.rightNode)) {
                    PortalNode* portal = dynamic_cast<PortalNode*>(nodeCard);

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
            ModifierCard* modifierCard = dynamic_cast<ModifierCard*>(choice.card);
            if (modifierCard != nullptr) {
                while (true) {
                    std::cout << "Enter the node you want to place the modifier on: ";
                    std::cin >> choice.targetNode;
                    if (bridge.isValidNode(choice.targetNode)) {
                        break;
                    }
                    std::cout << "Not valid, please try again." << std::endl;
                }
            }
        }

        choice.valid = true;

        std::cout << "Player "
                  << player.getPlayerId()
                  << " selected: "
                  << choice.card->getName()
                  << std::endl;

        break;
    }

    return choice;
}

void GameLogic::linkPortal(PortalNode* newPortal) {
    if (newPortal == nullptr) {
        return;
    }
    Node* currentNode = bridge.getPlayer1Castle();
    while(currentNode != nullptr) {
        PortalNode* existingPortal = dynamic_cast<PortalNode*>(currentNode->nodeCard);
        
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

bool GameLogic::applyPlacement(placementChoice& choice) {
    if (!choice.valid || choice.card == nullptr || choice.player == nullptr) {
        return false;
    }

    Dynamite* dynamite = dynamic_cast<Dynamite*>(choice.card);
    if (dynamite != nullptr) {
        Node* targetNode = bridge.getPlayer1Castle();
        for (int i = 1; i < choice.targetNode; i++) {
            targetNode = targetNode->right;
        }

        if (targetNode == player1.getCurrentNode() || targetNode == player2.getCurrentNode()) {
            std::cout << "Cannot remove node with player on it." << std::endl;

            return false;
        }

        bool succeeded = bridge.removeNode(choice.targetNode);
        if (!succeeded) {
            std::cout << "Failed to remove node at index " << choice.targetNode << std::endl;
            return false;
        }
        choice.player->removeCardFromHand(choice.card);
        delete dynamite;
        choice.card = nullptr;

        return true;
    }

    NodeCard* nodeCard = dynamic_cast<NodeCard*>(choice.card);
    if (nodeCard != nullptr) {
        bool succeeded = bridge.insertCard(choice.leftNode, choice.rightNode, nodeCard);
        if (!succeeded) {
            return false;
        }

        PortalNode* newPortal = dynamic_cast<PortalNode*>(nodeCard);
        if (newPortal != nullptr) {
            linkPortal(newPortal);
        }

        choice.player->removeCardFromHand(choice.card);
        choice.card = nullptr;

        return true;
    }

    ModifierCard* modifierCard = dynamic_cast<ModifierCard*>(choice.card);
    if (modifierCard != nullptr) {
        bool succeeded = bridge.attachModifierCard(choice.targetNode, modifierCard);
        if (!succeeded) {
            return false;
        }

        choice.player->removeCardFromHand(choice.card);
        choice.card = nullptr;

        return true;
    }

    return false;
}

void GameLogic::resolvePlacements(placementChoice& p1choice, placementChoice& p2choice) {
    NodeCard* p1NodeCard = dynamic_cast<NodeCard*>(p1choice.card);
    NodeCard* p2NodeCard = dynamic_cast<NodeCard*>(p2choice.card);

    if (p1NodeCard != nullptr && p2NodeCard != nullptr) {
        bool sameGap = (p1choice.leftNode == p2choice.leftNode && p1choice.rightNode == p2choice.rightNode);
        
        if (sameGap) {
            applyPlacement(p2choice);
            applyPlacement(p1choice);
            return;
        }

        if (p1choice.leftNode > p2choice.leftNode) {
            applyPlacement(p1choice);
            applyPlacement(p2choice);
        } else {
            applyPlacement(p2choice);
            applyPlacement(p1choice);
        }

        return;
    }

    if (p1NodeCard != nullptr) {
        applyPlacement(p2choice);
        applyPlacement(p1choice);
        return;
    }

    if (p2NodeCard != nullptr) {
        applyPlacement(p1choice);
        applyPlacement(p2choice);
        return;
    }

    applyPlacement(p1choice);
    applyPlacement(p2choice);
}

void GameLogic::movePlayer(Player& player, int amount) {

    Node* currentNode = player.getCurrentNode();

    bool movingForward = amount > 0;
    int steps;

    if (amount < 0) {
        steps = -amount;
    } else {
        steps = amount;
    }

    for (int i = 0; i < steps; i++) {
        Node* nextNode = nullptr;

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

        if (nextNode == nullptr) {
            break;
        }

        currentNode = nextNode;
    }

    player.setCurrentNode(currentNode);

}

int GameLogic::getNodeIndex(const Node* targetNode) const {
    Node* currentNode = bridge.getPlayer1Castle();
    int index = 1;

    while (currentNode != nullptr) {
        if (currentNode == targetNode) {
            return index;
        }

        currentNode = currentNode->right;
        index++;
    }

    return -1;
}

int GameLogic::calculateMovement(Node* node) {
    if (node == nullptr) {
        return 0;
    }

    int movement = 0;

    if (node->nodeCard != nullptr) {
        DiceNode* dicenode = dynamic_cast<DiceNode*>(node->nodeCard);
        if (dicenode != nullptr) {
        movement = dicenode->rollDice();
        } else {
        movement = node->nodeCard->getMovementAmount();
        }
    }

    ModifierStrand* currentModifier = node->beginningOfStrand;

    while (currentModifier != nullptr) {
        
        Multiplier* multiplier = dynamic_cast<Multiplier*>(currentModifier->modifierCard);
        if (multiplier != nullptr) {
            movement *= multiplier->getMultiplierAmount();
        }

        currentModifier = currentModifier->next;
    }

    currentModifier = node->beginningOfStrand;

    while (currentModifier != nullptr) {

        Booster* booster = dynamic_cast<Booster*>(currentModifier->modifierCard);
        Recoiler* recoiler = dynamic_cast<Recoiler*>(currentModifier->modifierCard);

        if (booster != nullptr) {
            movement += booster->getBoostAmount();
        } else if (recoiler != nullptr) {
            movement += recoiler->getRecoilAmount();
        }

        currentModifier = currentModifier->next;
    }

    return movement;
}

void GameLogic::movingPhase(Player& player)
{
    std::cout
        << "Moving phase for player "
        << player.getPlayerId()
        << std::endl;

    player.resetActivatedCardsForNewTurn();

    movePlayer(player, 1);

    while (true)
    {
        Node* landedNode = player.getCurrentNode();

        if (landedNode == nullptr)
        {
            break;
        }

        if (landedNode->isPlayer1Castle ||
            landedNode->isPlayer2Castle)
        {
            break;
        }

        if (player.hasActivated(landedNode))
        {
            std::cout
                << "This node already activated for Player "
                << player.getPlayerId()
                << "."
                << std::endl;

            break;
        }

        player.markActivated(landedNode);

        PortalNode* portal = dynamic_cast<PortalNode*>(landedNode->nodeCard);
        if (portal != nullptr && portal->getConnectedPortal() != nullptr) {
            Node* destination = bridge.getPlayer1Castle();

            while (destination != nullptr && destination -> nodeCard != portal->getConnectedPortal()) {
                destination = destination->right;
            }

            if (destination != nullptr) {
                player.setCurrentNode(destination);
                std::cout << "Player " << player.getPlayerId() << " teleported to the connected portal." << std::endl;
                break;
            }
        }

        int movement = calculateMovement(landedNode);
        
        std::string landedName = "Empty node";

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

        if (movement == 0)
        {
            break;
        }

        movePlayer(player, movement);
    }
}

void GameLogic::checkWinCondition() {
    bool p1ReachedCastle = (player1.getCurrentNode() == bridge.getPlayer2Castle());
    bool p2ReachedCastle = (player2.getCurrentNode() == bridge.getPlayer1Castle());

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