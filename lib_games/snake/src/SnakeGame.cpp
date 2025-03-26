/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** Snake game implementation
*/

#include "SnakeGame.hpp"
#include <iostream>

namespace arcd {

    SnakeGame::SnakeGame()
        : name("Snake"),
          score(0),
          gameOver(false)
    {
    }

    void SnakeGame::init()
    {
        std::cout << "Snake game initialized" << std::endl;
        score = 0;
        gameOver = false;
    }

    void SnakeGame::update(float deltaTime)
    {
        // nothing to do yet
        (void)deltaTime;
    }

    void SnakeGame::render(IGraphicsModule &graphics)
    {
        graphics.clear();
        graphics.drawText("SNAKE GAME", 350, 50);
        // simple border for the game area
        graphics.drawShape(ShapeType::Rectangle, 200, 100, 400, 400);
        // simple snake (just a few segments for testing)
        graphics.drawShape(ShapeType::Rectangle, 300, 300, 20, 20);
        graphics.drawShape(ShapeType::Rectangle, 280, 300, 20, 20);
        graphics.drawShape(ShapeType::Rectangle, 260, 300, 20, 20);
        // food item
        graphics.drawShape(ShapeType::Circle, 400, 250, 10, 10);
        // score
        graphics.drawText("Score: " + std::to_string(score), 350, 520);
        graphics.display();
    }

    bool SnakeGame::isGameOver() const
    {
        return gameOver;
    }

    int SnakeGame::getScore() const
    {
        return score;
    }

    const std::string &SnakeGame::getName() const
    {
        return name;
    }

}

extern "C" {
    arcd::IGameModule* create() {
        return new arcd::SnakeGame();
    }
}
