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