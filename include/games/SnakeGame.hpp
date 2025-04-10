#pragma once

#include "../interfaces/IGameLibrary.hpp"
#include <vector>
#include <deque>
#include <random>

namespace arcd {

struct Point {
    int x;
    int y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

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
    static constexpr int BOARD_WIDTH = 30;
    static constexpr int BOARD_HEIGHT = 20;
    static constexpr int INITIAL_SNAKE_LENGTH = 4;
    static constexpr int UPDATE_INTERVAL = 100; // milliseconds

    void spawnFood();
    bool checkCollision(const Point& point) const;
    void moveSnake();

    std::deque<Point> _snake;
    Point _food;
    Direction _direction;
    Direction _nextDirection;
    bool _gameOver;
    int _score;
    int _updateCounter;
    std::mt19937 _rng;
};

} // namespace arcd 