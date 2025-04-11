#include "games/SnakeGame.hpp"
#include "interfaces/IEvent.hpp"
#include "utils/KeyCodes.hpp"
#include <random>
#include <chrono>
#include <algorithm>
#include <iostream>

namespace arcd {

using namespace arcd;  // To access IGraphicsLibrary constants

SnakeGame::SnakeGame()
    : _direction(Direction::RIGHT)
    , _nextDirection(Direction::RIGHT)
    , _gameOver(false)
    , _score(0)
    , _updateAccumulator(0.0)
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
    _updateAccumulator = 0.0;
    
    spawnFood();
    
    // Clear game state cache so it will be regenerated
    _gameState.reset();
}

void SnakeGame::update(double deltaTime)
{
    if (_gameOver) return;
    
    _updateAccumulator += deltaTime;
    
    if (_updateAccumulator < UPDATE_INTERVAL) return;
    
    _updateAccumulator -= UPDATE_INTERVAL;
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
    
    // Clear game state cache so it will be regenerated
    _gameState.reset();
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

bool SnakeGame::processEvent(const IEvent& event)
{
    if (_gameOver) {
        // Check for restart
        if (event.getType() == EventType::KEY_PRESSED && event.getKeyCode() == KeyCode::RESTART) {
            restart();
            return true;
        }
        return false;
    }
    
    if (event.getType() != EventType::KEY_PRESSED) {
        return false;
    }
    
    switch (event.getKeyCode()) {
        case KeyCode::UP:
            if (_direction != Direction::DOWN) {
                _nextDirection = Direction::UP;
            }
            return true;
        case KeyCode::DOWN:
            if (_direction != Direction::UP) {
                _nextDirection = Direction::DOWN;
            }
            return true;
        case KeyCode::LEFT:
            if (_direction != Direction::RIGHT) {
                _nextDirection = Direction::LEFT;
            }
            return true;
        case KeyCode::RIGHT:
            if (_direction != Direction::LEFT) {
                _nextDirection = Direction::RIGHT;
            }
            return true;
        default:
            return false;
    }
}

std::unique_ptr<IGameState> SnakeGame::getGameState() const
{
    // If we already have a cached state and nothing changed, return a copy of it
    if (_gameState) {
        return std::make_unique<GameState>(*static_cast<GameState*>(_gameState.get()));
    }
    
    // Create a new game state
    auto state = std::make_unique<GameState>(BOARD_WIDTH, BOARD_HEIGHT);
    
    // Set basic game information
    state->setScore(_score);
    state->setGameOver(_gameOver);
    
    if (_gameOver) {
        state->setMessage("Game Over! Press 'r' to restart");
    }
    
    // Add snake entities
    for (size_t i = 0; i < _snake.size(); i++) {
        const auto& segment = _snake[i];
        std::string colorName = (i == 0) ? "GREEN" : "WHITE"; // Head is green, body is white
        
        auto entity = GameState::createEntity(
            EntityType::PLAYER,
            segment.x,
            segment.y,
            "█", // Use a block character for the snake
            colorName
        );
        
        // For the head, store its direction in properties
        if (i == 0) {
            entity.properties["isHead"] = true;
            entity.properties["direction"] = static_cast<int>(_direction);
        }
        
        state->addEntity(entity);
    }
    
    // Add food entity
    auto foodEntity = GameState::createEntity(
        EntityType::COLLECTIBLE,
        _food.x,
        _food.y,
        "█", // Use a block character for food too
        "RED"
    );
    state->addEntity(foodEntity);
    
    // Cache this state
    _gameState = std::make_unique<GameState>(*state);
    
    return state;
}

void SnakeGame::spawnFood()
{
    std::uniform_int_distribution<int> distX(0, BOARD_WIDTH - 1);
    std::uniform_int_distribution<int> distY(0, BOARD_HEIGHT - 1);
    
    do {
        _food = {distX(_rng), distY(_rng)};
    } while (std::find(_snake.begin(), _snake.end(), _food) != _snake.end());
    
    // Clear game state cache so it will be regenerated
    _gameState.reset();
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

std::string SnakeGame::getDescription() const
{
    return "Classic Snake game - eat food, grow longer, don't hit the walls or yourself!";
}

} // namespace arcd

extern "C" {
    std::unique_ptr<arcd::IGameLibrary> createGameLibrary() {
        return std::make_unique<arcd::SnakeGame>();
    }
    
    void destroyGameLibrary([[maybe_unused]] arcd::IGameLibrary* gameLib) {
        // With smart pointers, this is not needed anymore
    }
} 