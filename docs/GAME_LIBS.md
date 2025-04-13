# Game Libraries Documentation

This document details the game libraries in the Arcade project, explaining how they work, their interfaces, and implementation details.

## Overview

Game libraries in the Arcade project encapsulate individual games that can run on the Arcade platform. Each game library is responsible for:

1. **Game Logic**: Implementing the rules and mechanics of the game
2. **Game State**: Managing the state of the game (positions, scores, etc.)
3. **Rendering Logic**: Determining what should be drawn on screen
4. **Input Processing**: Responding to user inputs

Each game library implements the same interface (`IGameLibrary`), allowing the Core engine to interact with them consistently.

## Available Game Libraries

The Arcade project includes several game libraries:

| Game | Description | Key Features |
|------|-------------|-------------|
| Snake | Classic snake game | Growing snake, food collection, collision detection |
| Nibbler | Similar to Snake with obstacles | Maze navigation, increased difficulty |
| Pacman | Maze navigation game | Enemies with AI, power-ups, score system |

## Game Library Interface

All game libraries implement the `IGameLibrary` interface defined in `include/interfaces/IGameLibrary.hpp`:

```cpp
class IGameLibrary {
public:
    virtual ~IGameLibrary() = default;
    
    // Initialization and cleanup
    virtual bool initialize() = 0;
    virtual void cleanup() = 0;
    
    // Game loop methods
    virtual void update(Core* core) = 0;
    virtual void render(IGraphicsLibrary* graphicsLib) = 0;
    
    // Input handling
    virtual void handleInput(int key) = 0;
    virtual void handleMouseInput(const IGraphicsLibrary::MouseEvent& event) = 0;
    
    // Game information
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Score management
    virtual int getScore() const = 0;
    virtual void resetGame() = 0;
    
    // Game state
    virtual bool isGameOver() const = 0;
};
```

## Mouse Input Support

The addition of `handleMouseInput` method allows games to process mouse events. This is particularly useful for:

- **Minesweeper**: Clicking to reveal cells or place flags
- **Card Games**: Selecting or dragging cards
- **Point-and-Click Games**: Interacting with game elements

Games that don't require mouse input can provide a minimal implementation:

```cpp
void MyGame::handleMouseInput(const IGraphicsLibrary::MouseEvent& event) 
{
    // Ignore mouse events in this game
    (void)event;
}
```

## Snake Game Implementation

Let's examine the Snake game implementation as an example.

### Key Classes and Components

#### `SnakeGame` Class

The main class implementing the `IGameLibrary` interface for the Snake game.

```cpp
class SnakeGame : public IGameLibrary {
private:
    // Game state
    bool _initialized;
    bool _gameOver;
    int _score;
    float _lastUpdateTime;
    float _moveInterval;
    
    // Snake representation
    struct Position {
        int x;
        int y;
        bool operator==(const Position& other) const;
    };
    
    std::deque<Position> _snake;
    Position _food;
    Direction _currentDirection;
    Direction _nextDirection;
    
    // Game board dimensions
    int _width;
    int _height;
    
    // Helper methods
    void initializeGame();
    void moveSnake();
    bool checkCollision(const Position& pos);
    void placeFood();
    
public:
    SnakeGame();
    ~SnakeGame() override;
    
    // IGameLibrary implementation
    bool initialize() override;
    void cleanup() override;
    void update(Core* core) override;
    void render(IGraphicsLibrary* graphicsLib) override;
    void handleInput(int key) override;
    void handleMouseInput(const IGraphicsLibrary::MouseEvent& event) override;
    std::string getName() const override;
    std::string getDescription() const override;
    int getScore() const override;
    void resetGame() override;
    bool isGameOver() const override;
};
```

### Initialization Process

The Snake game initialization process:

1. Set up the initial game state (score = 0, not game over)
2. Create the initial snake (typically 3 segments in the middle)
3. Set the initial movement direction
4. Generate the first food position
5. Initialize timing variables for movement

```cpp
bool SnakeGame::initialize() {
    if (_initialized)
        return true;
    
    _width = 40;
    _height = 30;
    
    initializeGame();
    
    _initialized = true;
    return true;
}

void SnakeGame::initializeGame() {
    // Reset game state
    _gameOver = false;
    _score = 0;
    _moveInterval = 0.2f; // 200ms between moves initially
    _lastUpdateTime = 0.0f;
    
    // Create initial snake
    _snake.clear();
    int startX = _width / 2;
    int startY = _height / 2;
    
    _snake.push_back({startX, startY});
    _snake.push_back({startX-1, startY});
    _snake.push_back({startX-2, startY});
    
    _currentDirection = Direction::RIGHT;
    _nextDirection = Direction::RIGHT;
    
    // Place initial food
    placeFood();
}
```

### Game Loop Implementation

The game loop consists of two primary methods:

#### `update` Method

Updates the game state based on elapsed time and user input.

```cpp
void SnakeGame::update(Core* core) {
    if (_gameOver || !_initialized)
        return;
    
    // Get current time
    float currentTime = getCurrentTimeSeconds();
    
    // Move the snake at regular intervals
    if (currentTime - _lastUpdateTime >= _moveInterval) {
        _lastUpdateTime = currentTime;
        
        // Update direction based on queued input
        _currentDirection = _nextDirection;
        
        // Move the snake
        moveSnake();
        
        // Check if the game is over after moving
        if (_gameOver) {
            core->setState(GameState::GAME_OVER);
            core->setScore(_score);
        }
    }
}
```

#### `render` Method

Renders the current game state using the provided graphics library.

```cpp
void SnakeGame::render(IGraphicsLibrary* graphicsLib) {
    if (!_initialized)
        return;
    
    // Draw the game board/border
    graphicsLib->drawBox(0, 0, _width, _height, IGraphicsLibrary::Color::WHITE);
    
    // Draw the snake
    for (size_t i = 0; i < _snake.size(); i++) {
        const Position& pos = _snake[i];
        // Head of snake is a different color
        IGraphicsLibrary::Color color = (i == 0) ? 
                                       IGraphicsLibrary::Color::YELLOW : 
                                       IGraphicsLibrary::Color::GREEN;
        
        graphicsLib->drawText(pos.x, pos.y, "O", color);
    }
    
    // Draw the food
    graphicsLib->drawText(_food.x, _food.y, "*", IGraphicsLibrary::Color::RED);
    
    // Draw the score
    std::string scoreText = "Score: " + std::to_string(_score);
    graphicsLib->drawText(1, _height + 1, scoreText, IGraphicsLibrary::Color::WHITE);
    
    // Draw game over message if needed
    if (_gameOver) {
        std::string gameOverText = "GAME OVER - Press Enter to restart";
        int centerX = (_width - gameOverText.length()) / 2;
        int centerY = _height / 2;
        
        graphicsLib->drawText(centerX, centerY, gameOverText, IGraphicsLibrary::Color::RED);
    }
}
```

### Input Handling

The Snake game processes user input to change the snake's direction:

```cpp
void SnakeGame::handleInput(int key) {
    if (!_initialized)
        return;
    
    // Handle direction changes
    // Prevent 180-degree turns (can't go directly back on yourself)
    if (key == IGraphicsLibrary::KEY_UP_CODE && _currentDirection != Direction::DOWN) {
        _nextDirection = Direction::UP;
    } else if (key == IGraphicsLibrary::KEY_DOWN_CODE && _currentDirection != Direction::UP) {
        _nextDirection = Direction::DOWN;
    } else if (key == IGraphicsLibrary::KEY_LEFT_CODE && _currentDirection != Direction::RIGHT) {
        _nextDirection = Direction::LEFT;
    } else if (key == IGraphicsLibrary::KEY_RIGHT_CODE && _currentDirection != Direction::LEFT) {
        _nextDirection = Direction::RIGHT;
    } else if (key == IGraphicsLibrary::KEY_ENTER_CODE && _gameOver) {
        resetGame();
    }
}
```

### Core Game Mechanics

#### Snake Movement

```cpp
void SnakeGame::moveSnake() {
    // Get the current head position
    Position head = _snake.front();
    Position newHead = head;
    
    // Calculate new head position based on direction
    switch (_currentDirection) {
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
    
    // Check for collisions with walls or self
    if (newHead.x < 0 || newHead.x >= _width || 
        newHead.y < 0 || newHead.y >= _height || 
        checkCollision(newHead)) {
        _gameOver = true;
        return;
    }
    
    // Check if snake eats food
    bool ateFood = (newHead.x == _food.x && newHead.y == _food.y);
    
    // Add new head to snake
    _snake.push_front(newHead);
    
    // If food wasn't eaten, remove the tail
    if (!ateFood) {
        _snake.pop_back();
    } else {
        // Food was eaten, increase score and place new food
        _score += 10;
        placeFood();
        
        // Speed up the game slightly as the score increases
        _moveInterval = std::max(0.05f, _moveInterval - 0.005f);
    }
}
```

#### Collision Detection

```cpp
bool SnakeGame::checkCollision(const Position& pos) {
    // Skip the last element of the snake when checking collisions
    // (because it will be removed in the next step unless food is eaten)
    for (size_t i = 0; i < _snake.size() - 1; i++) {
        if (_snake[i].x == pos.x && _snake[i].y == pos.y) {
            return true;
        }
    }
    return false;
}
```

#### Food Placement

```cpp
void SnakeGame::placeFood() {
    // Try to place food in a random position not occupied by the snake
    Position newFood;
    bool validPosition = false;
    
    while (!validPosition) {
        newFood.x = rand() % _width;
        newFood.y = rand() % _height;
        
        // Check if this position is free
        validPosition = true;
        for (const auto& segment : _snake) {
            if (segment.x == newFood.x && segment.y == newFood.y) {
                validPosition = false;
                break;
            }
        }
    }
    
    _food = newFood;
}
```

## External Entry Points

Each game library provides C-style functions for dynamic loading:

```cpp
extern "C" {
    arcd::IGameLibrary* createGameLibrary() {
        return new arcd::SnakeGame();
    }

    void destroyGameLibrary(arcd::IGameLibrary* gameLib) {
        delete gameLib;
    }
}
```

## Game Development Guidelines

When creating a new game library for the Arcade platform:

1. **Implement the Interface**: Ensure all required methods from `IGameLibrary` are implemented
2. **Self-Contained Logic**: Keep game logic self-contained within the library
3. **Graphics Abstraction**: Use only the provided graphics interface methods for rendering
4. **Responsive Input**: Make input handling responsive by queuing complicated actions
5. **Resource Management**: Clean up resources in the `cleanup()` method
6. **Performance**: Keep the game loop efficient for smooth gameplay
7. **Error Handling**: Handle errors gracefully without crashing

## Next Steps

For more information on related components:
- [Core Engine Documentation](CORE.md)
- [Graphics Libraries Documentation](GRAPHICS_LIBS.md)
- [Interfaces Documentation](INTERFACES.md) 