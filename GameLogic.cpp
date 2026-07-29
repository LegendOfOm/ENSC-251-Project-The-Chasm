#include "Player.hpp"
#include "Bridge.hpp"
#include "GameLogic.hpp"
#include "Card.hpp"
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
    placingPhase(player1);
    placingPhase(player2);
    movingPhase(player1);
    movingPhase(player2);
    turnNumber++;
}

void GameLogic::drawingPhase(Player& player) {
    std::cout << "Drawing phase for player " << player.getPlayerId() << std::endl;
}

void GameLogic::placingPhase(Player& player) {
    std::cout << "Placing phase for player " << player.getPlayerId() << std::endl;
    for (int i = 0; i < player.getHandSize(); i++) {
        if (player.getHandSize() == 0) {
            std::cout << "No cards in hand" << std::endl;
            break;
        }
        if (player.getCardInHand(i) == nullptr) {
            std::cout << "No card in hand" << std::endl;
        } else {
            std::cout << "Card in hand: " << player.getCardInHand(i)->getName() << std::endl;
        }
    }
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