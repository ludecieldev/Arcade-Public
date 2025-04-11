#ifndef SNAKE_GAME_HPP
#define SNAKE_GAME_HPP

#include "../interfaces/IGameLibrary.hpp"
#include "../utils/GameState.hpp"
#include "../utils/KeyCodes.hpp"
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
    void update(double deltaTime) override;
    void restart() override;
    void cleanup() override;

    // Input handling
    bool processEvent(const IEvent& event) override;

    // Game state
    std::unique_ptr<IGameState> getGameState() const override;

    // Game information
    std::string getName() const override;
    std::string getDescription() const override;
    
    // Additional methods for game state query
    bool isGameOver() const;
    int getScore() const;

private:
    static constexpr int BOARD_WIDTH = 30;
    static constexpr int BOARD_HEIGHT = 20;
    static constexpr int INITIAL_SNAKE_LENGTH = 4;
    static constexpr double UPDATE_INTERVAL = 0.1; // seconds

    void spawnFood();
    bool checkCollision(const Point& point) const;
    void moveSnake();
    void updateEntities();

    std::deque<Point> _snake;
    Point _food;
    Direction _direction;
    Direction _nextDirection;
    bool _gameOver;
    int _score;
    double _updateAccumulator;
    std::mt19937 _rng;
    
    // Game state cache
    mutable std::unique_ptr<GameState> _gameState;
};

} // namespace arcd

#endif // SNAKE_GAME_HPP 