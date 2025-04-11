# Interfaces Documentation

This document provides detailed information about the interfaces used in the Arcade project, which define the contracts between different components of the system.

## Overview

Interfaces in the Arcade project serve as the foundation for its modular architecture, allowing different components to interact with each other without direct dependencies. The key interfaces are:

1. **IGraphicsLibrary**: Interface for graphics rendering libraries
2. **IGameLibrary**: Interface for game logic libraries
3. **IGameState**: Interface for representing game state
4. **IEvent**: Interface for input events

These interfaces enable the Core engine to interact with various implementations of games and graphics libraries in a consistent manner and maintain a clean separation of concerns.

## IGraphicsLibrary Interface

The `IGraphicsLibrary` interface provides methods for rendering and input handling.

### Key Components

```cpp
class IGraphicsLibrary {
public:
    virtual ~IGraphicsLibrary() = default;
    
    // Lifecycle management
    virtual bool initialize() = 0;
    virtual void cleanup() = 0;
    
    // Display management
    virtual void clear() = 0;
    virtual void refresh() = 0;
    
    // Game state rendering
    virtual void renderGameState(const IGameState& state) = 0;
    
    // UI rendering
    virtual void renderUI(const std::vector<UIElement>& elements) = 0;
    
    // Input handling
    virtual std::optional<std::unique_ptr<IEvent>> pollEvent() = 0;
    
    // Player interaction
    virtual void getPlayerName(std::string& playerName) = 0;
    
    // Window information
    virtual std::string getName() const = 0;
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

#### Rendering

- `void renderGameState(const IGameState& state)`: Renders the current game state.
- `void renderUI(const std::vector<UIElement>& elements)`: Renders UI elements like menus.

#### Input Handling

- `std::optional<std::unique_ptr<IEvent>> pollEvent()`: Returns the next available event, or std::nullopt if no event is available.
- `void getPlayerName(std::string& playerName)`: Implements player name input handling, updating the provided string.

#### Information Functions

- `std::string getName() const`: Returns the name of the graphics library.
- `int getWidth() const`: Returns the width of the display area.
- `int getHeight() const`: Returns the height of the display area.

## IGameLibrary Interface

The `IGameLibrary` interface provides methods for game logic and state management.

### Key Components

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

### Method Descriptions

#### Game Lifecycle

- `void initialize()`: Sets up the game, initializing game state, resources, etc.
- `void update(double deltaTime)`: Updates the game state based on elapsed time.
- `void restart()`: Resets the game to its initial state.
- `void cleanup()`: Cleans up resources used by the game.

#### Input Handling

- `bool processEvent(const IEvent& event)`: Processes an input event from the user. Returns true if the event was handled.

#### Game State

- `std::unique_ptr<IGameState> getGameState() const`: Returns the current state of the game.

#### Information Functions

- `std::string getName() const`: Returns the name of the game.
- `std::string getDescription() const`: Returns a description of the game.
- `bool isGameOver() const`: Returns true if the game is over.
- `int getScore() const`: Returns the current score.

## IGameState Interface

The `IGameState` interface represents the current state of a game in a way that can be rendered by any graphics library.

### Key Components

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

### Method Descriptions

- `int getScore() const`: Returns the current score.
- `bool isGameOver() const`: Returns true if the game is over.
- `std::string getMessage() const`: Returns any message to display (e.g., game over message).
- `int getWidth() const`: Returns the width of the game board.
- `int getHeight() const`: Returns the height of the game board.
- `const std::vector<Entity>& getEntities() const`: Returns all entities to be rendered.

## IEvent Interface

The `IEvent` interface represents input events in a way that's independent of the graphics library.

### Key Components

```cpp
enum class EventType {
    KEY_PRESSED,
    TEXT_ENTERED,
    MOUSE_PRESSED,
    MOUSE_RELEASED,
    MOUSE_MOVED,
    WINDOW_CLOSED
};

enum class KeyCode {
    UNKNOWN,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    ENTER,
    ESC,
    BACKSPACE,
    NEXT_LIB,
    NEXT_GAME,
    QUIT,
    RESTART
};

enum class MouseButton {
    NONE,
    LEFT,
    RIGHT,
    MIDDLE
};

class IEvent {
public:
    virtual ~IEvent() = default;
    
    virtual EventType getType() const = 0;
    virtual KeyCode getKeyCode() const = 0;
    virtual char getCharacter() const = 0;
    
    // Mouse event information
    virtual int getMouseX() const = 0;
    virtual int getMouseY() const = 0;
    virtual MouseButton getMouseButton() const = 0;
};
```

### Method Descriptions

- `EventType getType() const`: Returns the type of event.
- `KeyCode getKeyCode() const`: Returns the key code for key events.
- `char getCharacter() const`: Returns the character for text input events.
- `int getMouseX() const`: Returns the X coordinate for mouse events.
- `int getMouseY() const`: Returns the Y coordinate for mouse events.
- `MouseButton getMouseButton() const`: Returns the button for mouse button events.

## UI Elements

UI Elements are used to represent menu items, text, and other UI components.

```cpp
enum class UIElementType {
    TEXT,
    RECT,
    LIST
};

enum class Color {
    DEFAULT,
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE
};

struct UIElement {
    UIElementType type;
    int x;
    int y;
    int width;
    int height;
    std::string text;
    Color color;
    bool selected;
    std::map<std::string, std::string> properties;
};
```

## Implementing the Interfaces

### Creating a Graphics Library

To create a new graphics library for the Arcade project:

1. Create a class that inherits from `IGraphicsLibrary`
2. Implement all required methods
3. Provide external C functions for dynamic loading:

```cpp
extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary() {
        return std::make_unique<YourGraphicsLibrary>();
    }

    void destroyGraphicsLibrary([[maybe_unused]] arcd::IGraphicsLibrary* graphicsLib) {
        // With smart pointers, this is not needed anymore
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
    std::unique_ptr<arcd::IGameLibrary> createGameLibrary() {
        return std::make_unique<YourGame>();
    }

    void destroyGameLibrary([[maybe_unused]] arcd::IGameLibrary* gameLib) {
        // With smart pointers, this is not needed anymore
    }
}
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