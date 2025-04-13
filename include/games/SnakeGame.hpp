#ifndef SNAKE_GAME_HPP
#define SNAKE_GAME_HPP

#include "interfaces/IGameLibrary.hpp"
#include <string>
#include <deque>
#include <random>
#include <chrono>

namespace arcd {

// Position struct for the snake and food
struct Position {
    int x;
    int y;
    
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

// Direction enum for the snake movement
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

    // Game lifecycle
    void initialize() override;
    void update() override;
    void restart() override;
    void cleanup() override;

    // Input handling
    void handleInput(int key) override;

    // Rendering
    void render(IGraphicsLibrary& graphicsLib) override;

    // Game state
    bool isGameOver() const override;
    int getScore() const override;
    std::string getName() const override;

private:
    // Game constants
    static constexpr int BOARD_WIDTH = 30;
    static constexpr int BOARD_HEIGHT = 20;
    static constexpr int INITIAL_SNAKE_LENGTH = 4;
    static constexpr int UPDATE_INTERVAL_MS = 150; // Milliseconds between updates

    // Game state
    bool _gameOver;
    int _score;
    
    // Snake properties
    std::deque<Position> _snake;
    Direction _direction;
    Direction _nextDirection;
    
    // Food property
    Position _food;
    
    // Time tracking
    std::chrono::time_point<std::chrono::high_resolution_clock> _lastUpdateTime;
    
    // Random number generator
    std::mt19937 _rng;
    
    // Helper methods
    void spawnFood();
    bool checkCollision(const Position& pos);
    void moveSnake();
    void drawBoard(IGraphicsLibrary& graphicsLib, int startX, int startY, int cellWidth, int cellHeight);
    void drawSnake(IGraphicsLibrary& graphicsLib, int startX, int startY, int cellWidth, int cellHeight);
    void drawFood(IGraphicsLibrary& graphicsLib, int startX, int startY, int cellWidth, int cellHeight);
    void drawInfo(IGraphicsLibrary& graphicsLib, int startX, int startY, int cellWidth, int cellHeight);
};

} // namespace arcd

#endif // SNAKE_GAME_HPP 