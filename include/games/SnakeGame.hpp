/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** SnakeGame
*/

#ifndef SNAKE_GAME_HPP
#define SNAKE_GAME_HPP

#include "interfaces/IGameLibrary.hpp"
#include <string>
#include <deque>
#include <random>
#include <chrono>

namespace arcd {

/**
 * @brief Position struct for the snake and food
 * @param x: x coordinate
 * @param y: y coordinate
 */
struct Position {
    int x;
    int y;
    
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

/**
 * @brief Direction enum for the snake movement
 * @param UP: up direction
 * @param DOWN: down direction
 * @param LEFT: left direction
 * @param RIGHT: right direction
 */
enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class SnakeGame : public IGameLibrary {
public:
    SnakeGame();
    ~SnakeGame() override = default;

    /**
     * @brief Initialize the game
     */
    void initialize() override;

    /**
     * @brief Update the game
     */
    void update() override;
    void restart() override;
    void cleanup() override;

    /**
     * @brief Handle the input
     * @param key: key pressed
     */
    void handleInput(int key) override;

    /**
     * @brief Render the game
     * @param graphicsLib: graphics library to use for rendering
     */
    void render(IGraphicsLibrary& graphicsLib) override;

    /**
     * @brief Check if the game is over
     * @return true if the game is over, false otherwise
     */
    bool isGameOver() const override;

    /**
     * @brief Get the score
     * @return score
     */
    int getScore() const override;

    /**
     * @brief Get the name of the game
     * @return name of the game
     */
    std::string getName() const override;
private:
    /**
     * @brief Width of the game board
     */
    static constexpr int BOARD_WIDTH = 30;
    
    /**
     * @brief Height of the game board
     */
    static constexpr int BOARD_HEIGHT = 20;
    
    /**
     * @brief Initial length of the snake
     */
    static constexpr int INITIAL_SNAKE_LENGTH = 4;
    
    /**
     * @brief Time interval between updates in milliseconds
     */
    static constexpr int UPDATE_INTERVAL_MS = 150;

    /**
     * @brief Flag indicating if the game is over
     */
    bool _gameOver;
    
    /**
     * @brief Flag indicating if the player requested to exit
     */
    bool _exitRequested;
    
    /**
     * @brief Current score
     */
    int _score;
    
    /**
     * @brief Snake body represented as a deque of positions
     */
    std::deque<Position> _snake;
    
    /**
     * @brief Current direction of the snake
     */
    Direction _direction;
    
    /**
     * @brief Next direction to move the snake
     */
    Direction _nextDirection;
    
    /**
     * @brief Position of the food
     */
    Position _food;
    
    /**
     * @brief Time of the last update
     */
    std::chrono::time_point<std::chrono::high_resolution_clock> _lastUpdateTime;
    
    /**
     * @brief Random number generator
     */
    std::mt19937 _rng;
    
    /**
     * @brief Spawn food at a random position
     */
    void spawnFood();
    
    /**
     * @brief Check if there is a collision at the given position
     * @param pos: position to check
     * @return true if there is a collision, false otherwise
     */
    bool checkCollision(const Position& pos);
    
    /**
     * @brief Move the snake in the current direction
     */
    void moveSnake();
    
    /**
     * @brief Draw the game board
     * @param graphicsLib: graphics library to use for rendering
     * @param startX: starting X position
     * @param startY: starting Y position
     * @param cellWidth: width of each cell
     * @param cellHeight: height of each cell
     */
    void drawBoard(IGraphicsLibrary& graphicsLib, int startX, int startY, int cellWidth, int cellHeight);
    
    /**
     * @brief Draw the snake
     * @param graphicsLib: graphics library to use for rendering
     * @param startX: starting X position
     * @param startY: starting Y position
     * @param cellWidth: width of each cell
     * @param cellHeight: height of each cell
     */
    void drawSnake(IGraphicsLibrary& graphicsLib, int startX, int startY, int cellWidth, int cellHeight);
    
    /**
     * @brief Draw the food
     * @param graphicsLib: graphics library to use for rendering
     * @param startX: starting X position
     * @param startY: starting Y position
     * @param cellWidth: width of each cell
     * @param cellHeight: height of each cell
     */
    void drawFood(IGraphicsLibrary& graphicsLib, int startX, int startY, int cellWidth, int cellHeight);
    
    /**
     * @brief Draw game information
     * @param graphicsLib: graphics library to use for rendering
     * @param startX: starting X position
     * @param startY: starting Y position
     * @param cellWidth: width of each cell
     * @param cellHeight: height of each cell
     */
    void drawInfo(IGraphicsLibrary& graphicsLib, int startX, int startY, int cellWidth, int cellHeight);
};

} // namespace arcd

#endif // SNAKE_GAME_HPP 