#include "Player.hpp"
#include "Bridge.hpp"
#include "GameLogic.hpp"
#include "Card.hpp"
#include "NodeCard.hpp"
#include "ModifierCard.hpp"
#include "Dynamite.hpp"
#include "BridgeVisual.hpp"
#include <iostream>

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
        std::cout << "Press 'Q' to quit or any other key to continue: ";
        std::cin >> choice;
        if (!gameOver && (choice == 'Q' || choice == 'q')) {
            gameOver = true;
            std::cout << "game over" << std::endl;
        }
    }
}

void GameLogic::runTurn() {
    std::cout << "turn " << turnNumber << std::endl;
    drawingPhase(player1);
    drawingPhase(player2);
    placementChoice p1choice = placingPhase(player1);
    placementChoice p2choice = placingPhase(player2);
    resolvePlacements(p1choice, p2choice);
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
        std::cout << "Player "
                  << player.getPlayerId()
                  << " has no cards to place."
                  << std::endl;

        return choice;
    }

    for (int i = 0; i < player.getHandSize(); i++)
    {
        Card* card = player.getCardInHand(i);

        if (card != nullptr)
        {
            std::cout << i
                      << ": "
                      << card->getName()
                      << std::endl;
        }
    }

    while (true)
    {
        std::cout
            << "Enter the index of the card you would like to place from 0 to "
            << player.getHandSize() - 1
            << ": ";

        std::cin >> choice.cardIndex;

        if (choice.cardIndex < 0 ||
            choice.cardIndex >= player.getHandSize())
        {
            std::cout << "Invalid index. Please try again."
                      << std::endl;
            continue;
        }

        choice.card = player.getCardInHand(choice.cardIndex);

        if (choice.card == nullptr)
        {
            std::cout << "No card exists at that index. Please try again."
                      << std::endl;
            continue;
        }

        NodeCard* nodeCard = dynamic_cast<NodeCard*>(choice.card);
        if (nodeCard != nullptr) {
            while(true){
                std::cout << "Enter the 2 adjacent nodes you want to place the card between: ";
                std::cin >> choice.leftNode >> choice.rightNode;
                if (bridge.isValidNodePlacement(choice.leftNode, choice.rightNode)) {
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

bool GameLogic::applyPlacement(placementChoice& choice) {
    if (!choice.valid || choice.card == nullptr || choice.player == nullptr) {
        return false;
    }

    Dynamite* dynamite = dynamic_cast<Dynamite*>(choice.card);
    if (dynamite != nullptr) {
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

void GameLogic::movingPhase(Player& player) {
    std::cout << "Moving phase for player " << player.getPlayerId() << std::endl;
}

void GameLogic::checkWinCondition() {
    if (player1.getCurrentNode() == player2.getCastleNode()) {
        std::cout << "Player 1 has won the game!" << std::endl;
        gameOver = true;
    } else if (player2.getCurrentNode() == player1.getCastleNode()) {
        std::cout << "Player 2 has won the game!" << std::endl;
        gameOver = true;
    }
}