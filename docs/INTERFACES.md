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
    static constexpr int KEY_UP_CODE = 259;           // Up arrow key
    static constexpr int KEY_DOWN_CODE = 258;         // Down arrow key
    static constexpr int KEY_LEFT_CODE = 260;         // Left arrow key
    static constexpr int KEY_RIGHT_CODE = 261;        // Right arrow key
    static constexpr int KEY_ENTER_CODE = 10;         // Enter/Return key
    static constexpr int KEY_ESC_CODE = 27;           // Escape key
    static constexpr int KEY_BACKSPACE_CODE = 127;    // Backspace key
    static constexpr int KEY_NEXT_LIB_CODE = '9';     // Key to switch to next graphics library
    static constexpr int KEY_NEXT_GAME_CODE = '7';    // Key to switch to next game
    static constexpr int KEY_SPACE_CODE = ' ';        // Space key
    static constexpr int KEY_TAB_CODE = '\t';         // Tab key
    static constexpr int KEY_RESTART_GAME = 'r';      // Key to restart the game
    
    // Mouse constants
    static constexpr int MOUSE_LEFT_BUTTON = 1;       // Left mouse button
    static constexpr int MOUSE_RIGHT_BUTTON = 2;      // Right mouse button
    static constexpr int MOUSE_MIDDLE_BUTTON = 3;     // Middle mouse button (scroll wheel)
    
    // Mouse event structure
    struct MouseEvent {
        int x;                // X coordinate
        int y;                // Y coordinate
        int button;           // Button code (MOUSE_LEFT_BUTTON, MOUSE_RIGHT_BUTTON, etc.)
        bool pressed;         // true if button is pressed, false if released
        bool hasEvent;        // true if there's a mouse event, false otherwise
    };
    
    // Colors for rendering
    enum class Color {
        DEFAULT, BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE
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
    
    // Standardized menu drawing
    virtual void drawMenu(
        const std::string& title,
        const std::vector<std::string>& gameOptions,
        const std::vector<std::string>& graphicOptions,
        const std::string& playerName,
        int selectedMenu,
        int selectedGameIndex,
        int selectedGraphicIndex) = 0;
    
    // Input handling
    virtual int getKey() = 0;
    virtual MouseEvent getMouse() = 0;
    
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
- `MouseEvent getMouse()`: Returns information about the last mouse event, including position, button pressed, and event state.
- `void getPlayerName(std::string& playerName)`: Implements player name input handling, updating the provided string.

#### Information Functions

- `std::string getName() const`: Returns the name of the graphics library.
- `int getWidth() const`: Returns the width of the display area.
- `int getHeight() const`: Returns the height of the display area.

### Key Codes

The `IGraphicsLibrary` interface defines standard key codes for consistent input handling across different graphics libraries:

| Constant | Value | Description |
|----------|-------|-------------|
| KEY_UP_CODE | 259 | Up arrow key |
| KEY_DOWN_CODE | 258 | Down arrow key |
| KEY_LEFT_CODE | 260 | Left arrow key |
| KEY_RIGHT_CODE | 261 | Right arrow key |
| KEY_ENTER_CODE | 10 | Enter/Return key |
| KEY_ESC_CODE | 27 | Escape key |
| KEY_BACKSPACE_CODE | 127 | Backspace key |
| KEY_NEXT_LIB_CODE | '9' | Key to switch to next graphics library |
| KEY_NEXT_GAME_CODE | '7' | Key to switch to next game |
| KEY_SPACE_CODE | ' ' | Space key |
| KEY_TAB_CODE | '\t' | Tab key |
| KEY_RESTART_GAME | 'r' | Key to restart the game |

### Mouse Constants

The interface also defines standard mouse button codes:

| Constant | Value | Description |
|----------|-------|-------------|
| MOUSE_LEFT_BUTTON | 1 | Left mouse button |
| MOUSE_RIGHT_BUTTON | 2 | Right mouse button |
| MOUSE_MIDDLE_BUTTON | 3 | Middle mouse button (scroll wheel) |

### Mouse Event Structure

The `MouseEvent` structure contains information about mouse events:

| Field | Type | Description |
|-------|------|-------------|
| x | int | X coordinate of the mouse position |
| y | int | Y coordinate of the mouse position |
| button | int | Button code (LEFT, RIGHT, MIDDLE) |
| pressed | bool | True if button is pressed, false if released |
| hasEvent | bool | True if a mouse event has occurred, false otherwise |

### Colors

The `Color` enum defines standard colors for rendering:

| Color | Description |
|-------|-------------|
| DEFAULT | Default color (typically white) |
| BLACK | Black color |
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