# Game Libraries Documentation

This document details the game libraries in the Arcade project, explaining how they work, their interfaces, and implementation details.

## Overview

Game libraries in the Arcade project encapsulate individual games that can run on the Arcade platform. Each game library is responsible for:

1. **Game Logic**: Implementing the rules and mechanics of the game
2. **Game State**: Managing the state of the game (positions, scores, etc.)
3. **Input Processing**: Responding to user inputs

Each game library implements the same interface (`IGameLibrary`), allowing the Core engine to interact with them consistently.

## Separation of Concerns

It's important to note that game libraries:
- Should NOT contain any rendering logic - they only provide game state
- Should NOT depend on any specific graphics library
- Should be completely interchangeable with other game libraries

## Available Game Libraries

The Arcade project includes several game libraries:

| Game | Description | Key Features |
|------|-------------|-------------|
| Snake | Classic snake game | Growing snake, food collection, collision detection |
| Nibbler | Similar to Snake with obstacles | Maze navigation, increased difficulty |

## Game Library Interface

All game libraries implement the `IGameLibrary` interface:

```cpp
class IGameLibrary {
public:
    virtual ~IGameLibrary() = default;

    // Game lifecycle
    virtual void initialize() = 0;
    virtual void update(double deltaTime) = 0;
    virtual void restart() = 0;
    virtual void cleanup() = 0;

    // Input handling
    virtual bool processEvent(const IEvent& event) = 0;

    // Game state
    virtual std::unique_ptr<IGameState> getGameState() const = 0;

    // Game information
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Additional methods for game state query
    virtual bool isGameOver() const = 0;
    virtual int getScore() const = 0;
};
```

## Game State Interface

Game libraries communicate with graphics libraries through the `IGameState` interface, which represents the current state of the game:

```cpp
class IGameState {
public:
    virtual ~IGameState() = default;
    
    // Game state access
    virtual int getScore() const = 0;
    virtual bool isGameOver() const = 0;
    virtual std::string getMessage() const = 0;
    
    // Board dimensions
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    
    // Entities in the game
    virtual const std::vector<Entity>& getEntities() const = 0;
};
```

## Snake Game Implementation

The Snake game implements the `IGameLibrary` interface:

### Key Features

- Classic snake gameplay
- The snake grows longer when it eats food
- Game over when the snake hits a wall or itself
- Score increases with each food eaten

### Implementation Details

The Snake game uses:
- A deque to represent the snake segments
- A simple 2D grid system for collision detection
- Random food placement
- Direction control via arrow keys

### Event Handling

The Snake game processes directional input from the keyboard:

```cpp
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
```

### Game State Management

The Snake game provides its state through the `getGameState` method:

```cpp
std::unique_ptr<IGameState> SnakeGame::getGameState() const
{
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
    
    return state;
}
```

## Creating a New Game Library

To create a new game library:

1. Create a class that inherits from `IGameLibrary`
2. Implement all required methods
3. Provide a way to represent the game state through the `IGameState` interface
4. Export the required creation and destruction functions:

```cpp
extern "C" {
    std::unique_ptr<arcd::IGameLibrary> createGameLibrary() {
        return std::make_unique<MyGame>();
    }
    
    void destroyGameLibrary([[maybe_unused]] arcd::IGameLibrary* gameLib) {
        // With smart pointers, this is not needed anymore
    }
}
```

## Best Practices

When implementing a game library:

1. **State Management**: Keep track of all game entities and their states
2. **Efficient Updates**: Only update when necessary (e.g., on timer or input)
3. **Clean Interfaces**: Provide clear, consistent game state to graphics libraries
4. **Error Handling**: Handle edge cases gracefully
5. **Resource Management**: Clean up any allocated resources on destruction
6. **Separation of Concerns**: Never include rendering code in your game library
7. **Performance**: Optimize for efficient state updates and minimal memory usage 