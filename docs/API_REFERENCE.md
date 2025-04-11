# Arcade Project API Reference

This document provides a comprehensive reference to the Arcade project's API, designed for developers implementing or extending the project's components.

## Table of Contents

- [Core API](#core-api)
- [IGraphicsLibrary API](#igraphicslibrary-api)
- [IGameLibrary API](#igamelibrary-api)
- [IGameState API](#igamestate-api)
- [IEvent API](#ievent-api)
- [Dynamic Loading API](#dynamic-loading-api)
- [Utility Functions](#utility-functions)

## Core API

The Core class is the central component that manages the application and mediates between graphics and game libraries.

### Core Class

```cpp
namespace arcd {

enum class CoreState {
    MENU,
    GAME,
    SELECT_GAME,
    SELECT_GRAPHICS,
    GAME_OVER,
    PAUSE,
    EXIT
};

class ICore {
public:
    virtual ~ICore() = default;

    // Core lifecycle
    virtual bool initialize() = 0;
    virtual void run() = 0;
    virtual void cleanup() = 0;
    
    // Event handling
    virtual void processEvents() = 0;

    // Library management
    virtual bool loadGameLibrary(const std::string& path) = 0;
    virtual bool loadGraphicsLibrary(const std::string& path) = 0;
    virtual bool switchGameLibrary(const std::string& name) = 0;
    virtual bool switchGraphicsLibrary(const std::string& name) = 0;
    
    // Resource access
    virtual std::vector<std::string> getAvailableGameLibraries() const = 0;
    virtual std::vector<std::string> getAvailableGraphicsLibraries() const = 0;
    virtual CoreState getState() const = 0;
    virtual void setState(CoreState state) = 0;
    virtual const IGameState* getCurrentGameState() const = 0;
    virtual std::vector<UIElement> getUIElements() const = 0;
    
    // Player management
    virtual void setPlayerName(const std::string& name) = 0;
    virtual std::string getPlayerName() const = 0;
    
    // Score management
    virtual void saveScore(const std::string& gameName, int score) = 0;
    virtual std::vector<std::pair<std::string, int>> getHighScores(const std::string& gameName) const = 0;
};

}
```

### Core Methods

#### Initialization and Cleanup

- `bool initialize()`: Initializes the Core, loading the default or specified libraries.
- `void run()`: Enters the main application loop, handling input, updating game state, and rendering.
- `void cleanup()`: Cleans up resources and unloads libraries.

#### Event Handling

- `void processEvents()`: Handles events from the graphics library, routing them appropriately.

#### Library Management

- `bool loadGameLibrary(const std::string& path)`: Loads a game library from the specified path.
- `bool loadGraphicsLibrary(const std::string& path)`: Loads a graphics library from the specified path.
- `bool switchGameLibrary(const std::string& name)`: Switches to a specific game library by name.
- `bool switchGraphicsLibrary(const std::string& name)`: Switches to a specific graphics library by name.

#### Resource Access

- `std::vector<std::string> getAvailableGameLibraries() const`: Gets names of available game libraries.
- `std::vector<std::string> getAvailableGraphicsLibraries() const`: Gets names of available graphics libraries.
- `CoreState getState() const`: Gets the current application state.
- `void setState(CoreState state)`: Sets the current application state.
- `const IGameState* getCurrentGameState() const`: Gets the current game state.
- `std::vector<UIElement> getUIElements() const`: Gets UI elements to be rendered.

#### Player and Score Management

- `void setPlayerName(const std::string& name)`: Sets the current player's name.
- `std::string getPlayerName() const`: Gets the current player's name.
- `void saveScore(const std::string& gameName, int score)`: Saves a score for the current player.
- `std::vector<std::pair<std::string, int>> getHighScores(const std::string& gameName)`: Gets high scores for a game.

## IGraphicsLibrary API

The IGraphicsLibrary interface defines the contract for graphics libraries.

### IGraphicsLibrary Interface

```cpp
namespace arcd {

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

enum class UIElementType {
    TEXT,
    RECT,
    LIST
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

}
```

### IGraphicsLibrary Methods

#### Lifecycle Management

- `bool initialize()`: Initializes the graphics library, setting up windows, resources, etc. Returns true if successful.
- `void cleanup()`: Cleans up resources used by the graphics library.

#### Display Management

- `void clear()`: Clears the screen or display buffer.
- `void refresh()`: Updates the display with current drawing commands.

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

## IGameLibrary API

The IGameLibrary interface defines the contract for game libraries.

### IGameLibrary Interface

```cpp
namespace arcd {

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

}
```

### IGameLibrary Methods

#### Game Lifecycle

- `void initialize()`: Initializes the game, setting up initial state and resources.
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

## IGameState API

The IGameState interface defines the representation of a game's current state.

### IGameState Interface

```cpp
namespace arcd {

enum class EntityType {
    PLAYER,
    ENEMY,
    COLLECTIBLE,
    OBSTACLE,
    PROJECTILE,
    EFFECT,
    BACKGROUND
};

struct Entity {
    EntityType type;
    int x;
    int y;
    std::string symbol;
    std::string colorName;
    std::map<std::string, std::string> properties;
};

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

}
```

### IGameState Methods

- `int getScore() const`: Returns the current score.
- `bool isGameOver() const`: Returns true if the game is over.
- `std::string getMessage() const`: Returns any message to display (e.g., game over message).
- `int getWidth() const`: Returns the width of the game board.
- `int getHeight() const`: Returns the height of the game board.
- `const std::vector<Entity>& getEntities() const`: Returns all entities to be rendered.

## IEvent API

The IEvent interface defines input events in a library-agnostic way.

### IEvent Interface

```cpp
namespace arcd {

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

}
```

### IEvent Methods

- `EventType getType() const`: Returns the type of event.
- `KeyCode getKeyCode() const`: Returns the key code for key events.
- `char getCharacter() const`: Returns the character for text input events.
- `int getMouseX() const`: Returns the X coordinate for mouse events.
- `int getMouseY() const`: Returns the Y coordinate for mouse events.
- `MouseButton getMouseButton() const`: Returns the button for mouse button events.

## Dynamic Loading API

The DLLoader class provides functionality for dynamically loading libraries.

### DLLoader Class

```cpp
namespace arcd {

template<typename T>
class DLLoader {
public:
    DLLoader();
    ~DLLoader();
    
    bool load(const std::string& path);
    void unload();
    
    T* getInstance() const;
    std::string getPath() const;
};

}
```

### DLLoader Methods

- `bool load(const std::string& path)`: Loads the library at the specified path, finding and calling the creation function. Returns true if successful.
- `void unload()`: Unloads the library, calling the destruction function and closing the library handle.
- `T* getInstance() const`: Returns a pointer to the instance created by the library.
- `std::string getPath() const`: Returns the path of the loaded library.

## External C Functions

Each library must export these C functions for dynamic loading:

### Graphics Libraries

```cpp
extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary();
    void destroyGraphicsLibrary([[maybe_unused]] arcd::IGraphicsLibrary* graphicsLib);
}
```

### Game Libraries

```cpp
extern "C" {
    std::unique_ptr<arcd::IGameLibrary> createGameLibrary();
    void destroyGameLibrary([[maybe_unused]] arcd::IGameLibrary* gameLib);
}
```

## Utility Functions

The Arcade project provides various utility functions for common tasks.

### File System Functions

```cpp
namespace arcd {
namespace util {

// Returns a list of files in a directory matching a pattern
std::vector<std::string> findFiles(const std::string& dirPath, const std::string& pattern);

// Checks if a file exists
bool fileExists(const std::string& path);

// Gets the base name of a file (without directory and extension)
std::string baseName(const std::string& path);

}
}
```

### String Functions

```cpp
namespace arcd {
namespace util {

// Splits a string by a delimiter
std::vector<std::string> split(const std::string& str, char delimiter);

// Checks if a string starts with a prefix
bool startsWith(const std::string& str, const std::string& prefix);

// Checks if a string ends with a suffix
bool endsWith(const std::string& str, const std::string& suffix);

// Trims whitespace from the beginning and end of a string
std::string trim(const std::string& str);

}
}
```

### Time Functions

```cpp
namespace arcd {
namespace util {

// Returns the current time in seconds
double getCurrentTimeSeconds();

// Returns the current time in milliseconds
int64_t getCurrentTimeMillis();

}
}
``` 