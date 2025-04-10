#include "games/SnakeGame.hpp"
#include "interfaces/IGraphicsLibrary.hpp"
#include <random>
#include <chrono>

namespace arcd {

using namespace arcd;  // To access IGraphicsLibrary constants

SnakeGame::SnakeGame()
    : _direction(Direction::RIGHT)
    , _nextDirection(Direction::RIGHT)
    , _gameOver(false)
    , _score(0)
    , _updateCounter(0)
    , _rng(std::random_device{}())
{
    initialize();
}

void SnakeGame::initialize()
{
    // Initialize snake in the middle of the board
    _snake.clear();
    int startX = BOARD_WIDTH / 2;
    int startY = BOARD_HEIGHT / 2;
    
    for (int i = 0; i < INITIAL_SNAKE_LENGTH; ++i) {
        _snake.push_front({startX - i, startY});
    }
    
    _direction = Direction::RIGHT;
    _nextDirection = Direction::RIGHT;
    _gameOver = false;
    _score = 0;
    _updateCounter = 0;
    
    spawnFood();
}

void SnakeGame::update()
{
    if (_gameOver) return;
    
    _updateCounter++;
    if (_updateCounter < UPDATE_INTERVAL / 50) return; // Update every 100ms
    _updateCounter = 0;
    
    _direction = _nextDirection;
    moveSnake();
    
    // Check if snake ate food
    if (_snake.front() == _food) {
        _score += 10;
        spawnFood();
    } else {
        _snake.pop_back();
    }
    
    // Check for collisions
    if (checkCollision(_snake.front())) {
        _gameOver = true;
    }
}

void SnakeGame::moveSnake()
{
    Point newHead = _snake.front();
    
    switch (_direction) {
        case Direction::UP:
            newHead.y--;
            break;
        case Direction::DOWN:
            newHead.y++;
            break;
        case Direction::LEFT:
            newHead.x--;
            break;
        case Direction::RIGHT:
            newHead.x++;
            break;
    }
    
    _snake.push_front(newHead);
}

void SnakeGame::handleInput(int key)
{
    switch (key) {
        case IGraphicsLibrary::KEY_UP_CODE:
            if (_direction != Direction::DOWN)
                _nextDirection = Direction::UP;
            break;
        case IGraphicsLibrary::KEY_DOWN_CODE:
            if (_direction != Direction::UP)
                _nextDirection = Direction::DOWN;
            break;
        case IGraphicsLibrary::KEY_LEFT_CODE:
            if (_direction != Direction::RIGHT)
                _nextDirection = Direction::LEFT;
            break;
        case IGraphicsLibrary::KEY_RIGHT_CODE:
            if (_direction != Direction::LEFT)
                _nextDirection = Direction::RIGHT;
            break;
    }
}

void SnakeGame::render(IGraphicsLibrary& graphicsLib)
{
    // Calculate board position to center it
    int startX = (graphicsLib.getWidth() - BOARD_WIDTH * 2) / 2;
    int startY = (graphicsLib.getHeight() - BOARD_HEIGHT) / 2;
    
    // Draw border
    graphicsLib.drawBox(startX - 1, startY - 1, BOARD_WIDTH * 2 + 2, BOARD_HEIGHT + 2, Color::CYAN);
    
    // Draw snake
    for (const auto& segment : _snake) {
        if (segment == _snake.front()) {
            graphicsLib.drawText(startX + segment.x * 2, startY + segment.y, "██", Color::GREEN);
        } else {
            graphicsLib.drawText(startX + segment.x * 2, startY + segment.y, "██", Color::WHITE);
        }
    }
    
    // Draw food
    graphicsLib.drawText(startX + _food.x * 2, startY + _food.y, "██", Color::RED);
    
    // Draw score
    std::string scoreText = "Score: " + std::to_string(_score);
    graphicsLib.drawText(startX, startY - 2, scoreText, Color::YELLOW);
    
    // Draw game over message
    if (_gameOver) {
        std::string gameOverText = "Game Over! Press R to restart";
        int textX = (graphicsLib.getWidth() - gameOverText.length()) / 2;
        graphicsLib.drawText(textX, startY + BOARD_HEIGHT + 2, gameOverText, Color::RED);
    }
}

void SnakeGame::spawnFood()
{
    std::uniform_int_distribution<int> distX(0, BOARD_WIDTH - 1);
    std::uniform_int_distribution<int> distY(0, BOARD_HEIGHT - 1);
    
    do {
        _food = {distX(_rng), distY(_rng)};
    } while (std::find(_snake.begin(), _snake.end(), _food) != _snake.end());
}

bool SnakeGame::checkCollision(const Point& point) const
{
    // Check wall collision
    if (point.x < 0 || point.x >= BOARD_WIDTH ||
        point.y < 0 || point.y >= BOARD_HEIGHT) {
        return true;
    }
    
    // Check self collision (skip head)
    auto it = _snake.begin();
    ++it;
    for (; it != _snake.end(); ++it) {
        if (*it == point) {
            return true;
        }
    }
    
    return false;
}

void SnakeGame::restart()
{
    initialize();
}

void SnakeGame::cleanup()
{
    // Nothing to clean up
}

bool SnakeGame::isGameOver() const
{
    return _gameOver;
}

int SnakeGame::getScore() const
{
    return _score;
}

std::string SnakeGame::getName() const
{
    return "Snake";
}

} // namespace arcd

extern "C" {
    std::unique_ptr<arcd::IGameLibrary> createGameLibrary() {
        return std::make_unique<arcd::SnakeGame>();
    }
    
    void destroyGameLibrary([[maybe_unused]] arcd::IGameLibrary* gameLib) {
        // With smart pointers, this function is not needed anymore
        // but we keep it for compatibility
    }
} 