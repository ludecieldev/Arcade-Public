# Interfaces Documentation

This document provides detailed information about the interfaces used in the Arcade project, which define the contracts between different components of the system.

## Overview

Interfaces in the Arcade project serve as the foundation for its modular architecture, allowing different components to interact with each other without direct dependencies. The key interfaces are:

1. **IGraphicsLibrary**: Interface for graphics rendering libraries
2. **IGameLibrary**: Interface for game logic libraries

These interfaces enable the Core engine to interact with various implementations of games and graphics libraries in a consistent manner.

## IGraphicsLibrary Interface

The `IGraphicsLibrary` interface is defined in `include/interfaces/IGraphicsLibrary.hpp` and provides methods for rendering and input handling.

### Key Components

```cpp
class IGraphicsLibrary {
public:
    // Common key codes for consistent input handling across graphics libraries
    static const int KEY_UP_CODE = -1;
    static const int KEY_DOWN_CODE = -2;
    static const int KEY_LEFT_CODE = -3;
    static const int KEY_RIGHT_CODE = -4;
    static const int KEY_ENTER_CODE = -5;
    static const int KEY_ESC_CODE = -6;
    static const int KEY_BACKSPACE_CODE = -7;
    static const int KEY_NEXT_LIB_CODE = -8;
    static const int KEY_NEXT_GAME_CODE = -9;
    
    // Colors for rendering
    enum class Color {
        DEFAULT, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE
    };
    
    virtual ~IGraphicsLibrary() = default;
    
    // Initialization and cleanup
    virtual bool initialize() = 0;
    virtual void cleanup() = 0;
    
    // Display functions
    virtual void clear() = 0;
    virtual void refresh() = 0;
    
    // Drawing functions
    virtual void drawText(int x, int y, const std::string& text, Color color = Color::DEFAULT) = 0;
    virtual void drawBox(int x, int y, int width, int height, Color color = Color::DEFAULT) = 0;
    virtual void drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color = Color::DEFAULT) = 0;
    
    // Input handling
    virtual int getKey() = 0;
    
    // Player name input
    virtual void getPlayerName(std::string& playerName) = 0;
    
    // Library information
    virtual std::string getName() const = 0;
    
    // Window dimensions
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
};
```

### Method Descriptions

#### Initialization and Cleanup

- `bool initialize()`: Sets up the graphics library, creating windows, loading resources, etc. Returns true if successful.
- `void cleanup()`: Cleans up resources used by the graphics library.

#### Display Control

- `void clear()`: Clears the screen/display buffer.
- `void refresh()`: Updates the display with any changes since the last refresh.

#### Drawing Functions

- `void drawText(int x, int y, const std::string& text, Color color)`: Draws text at position (x,y) with the specified color.
- `void drawBox(int x, int y, int width, int height, Color color)`: Draws a rectangular box with the top-left corner at (x,y) and the specified dimensions.
- `void drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color)`: Draws a list of items with the selected item highlighted.

#### Input Handling

- `int getKey()`: Returns the key code of the last key pressed, or 0 if no key has been pressed.
- `void getPlayerName(std::string& playerName)`: Implements player name input handling, updating the provided string.

#### Information Functions

- `std::string getName() const`: Returns the name of the graphics library.
- `int getWidth() const`: Returns the width of the display area.
- `int getHeight() const`: Returns the height of the display area.

### Key Codes

The `IGraphicsLibrary` interface defines standard key codes for consistent input handling across different graphics libraries:

| Constant | Value | Description |
|----------|-------|-------------|
| KEY_UP_CODE | -1 | Up arrow key |
| KEY_DOWN_CODE | -2 | Down arrow key |
| KEY_LEFT_CODE | -3 | Left arrow key |
| KEY_RIGHT_CODE | -4 | Right arrow key |
| KEY_ENTER_CODE | -5 | Enter/Return key |
| KEY_ESC_CODE | -6 | Escape key |
| KEY_BACKSPACE_CODE | -7 | Backspace key |
| KEY_NEXT_LIB_CODE | -8 | Key to switch to next graphics library |
| KEY_NEXT_GAME_CODE | -9 | Key to switch to next game |

### Colors

The `Color` enum defines standard colors for rendering:

| Color | Description |
|-------|-------------|
| DEFAULT | Default color (typically white) |
| RED | Red color |
| GREEN | Green color |
| YELLOW | Yellow color |
| BLUE | Blue color |
| MAGENTA | Magenta color |
| CYAN | Cyan color |
| WHITE | White color |

## IGameLibrary Interface

The `IGameLibrary` interface is defined in `include/interfaces/IGameLibrary.hpp` and provides methods for game logic and rendering.

### Key Components

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

### Method Descriptions

#### Initialization and Cleanup

- `bool initialize()`: Sets up the game, initializing game state, resources, etc. Returns true if successful.
- `void cleanup()`: Cleans up resources used by the game.

#### Game Loop

- `void update(Core* core)`: Updates the game state based on elapsed time and input. The Core parameter allows the game to interact with the application.
- `void render(IGraphicsLibrary* graphicsLib)`: Renders the current game state using the provided graphics library.

#### Input Handling

- `void handleInput(int key)`: Processes a key input from the user.

#### Information Functions

- `std::string getName() const`: Returns the name of the game.
- `std::string getDescription() const`: Returns a description of the game.

#### Game State Management

- `int getScore() const`: Returns the current score.
- `void resetGame()`: Resets the game to its initial state.
- `bool isGameOver() const`: Returns true if the game is over.

## Implementing the Interfaces

### Creating a Graphics Library

To create a new graphics library for the Arcade project:

1. Create a class that inherits from `IGraphicsLibrary`
2. Implement all required methods
3. Provide external C functions for dynamic loading:

```cpp
extern "C" {
    arcd::IGraphicsLibrary* createGraphicsLibrary() {
        return new YourGraphicsLibrary();
    }

    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* graphicsLib) {
        delete graphicsLib;
    }
}
```

### Creating a Game Library

To create a new game for the Arcade project:

1. Create a class that inherits from `IGameLibrary`
2. Implement all required methods
3. Provide external C functions for dynamic loading:

```cpp
extern "C" {
    arcd::IGameLibrary* createGameLibrary() {
        return new YourGame();
    }

    void destroyGameLibrary(arcd::IGameLibrary* gameLib) {
        delete gameLib;
    }
}
```

## Additional Interfaces

### IDisplayable Interface (Optional)

Some implementations may use an additional `IDisplayable` interface for elements that can be rendered:

```cpp
class IDisplayable {
public:
    virtual ~IDisplayable() = default;
    
    virtual void render(IGraphicsLibrary* graphicsLib) = 0;
    virtual void update(float deltaTime) = 0;
};
```

## Interface Design Principles

The interfaces in the Arcade project follow these design principles:

1. **Separation of Concerns**: Each interface focuses on a specific responsibility
2. **Minimal Coupling**: Interfaces minimize dependencies between components
3. **Consistent Naming**: Method names are consistent across interfaces
4. **Clear Contracts**: Each method has clear inputs, outputs, and side effects
5. **Error Handling**: Methods include appropriate error handling mechanisms

## Next Steps

For more information on components that implement these interfaces:
- [Core Engine Documentation](CORE.md)
- [Graphics Libraries Documentation](GRAPHICS_LIBS.md)
- [Game Libraries Documentation](GAME_LIBS.md) 