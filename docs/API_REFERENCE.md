# Arcade Project API Reference

This document provides a comprehensive reference to the Arcade project's API, designed for developers implementing or extending the project's components.

## Table of Contents

- [Core API](#core-api)
- [IGraphicsLibrary API](#igraphicslibrary-api)
- [IGameLibrary API](#igamelibrary-api)
- [Dynamic Loading API](#dynamic-loading-api)
- [Utility Functions](#utility-functions)

## Core API

The Core class is the central component that manages the application and mediates between graphics and game libraries.

### Core Class

```cpp
namespace arcd {

enum class GameState {
    MENU,
    PLAYING,
    GAME_OVER,
    HIGH_SCORES
};

class Core {
public:
    Core();
    ~Core();
    
    // Main application control
    void initialize();
    void run();
    void cleanup();
    
    // State management
    void setState(GameState state);
    GameState getState() const;
    
    // Library management
    void loadLibraries();
    void nextGraphicsLib();
    void nextGameLib();
    void switchGraphicsLib(const std::string& path);
    void switchGameLib(const std::string& path);
    
    // Game data management
    void setPlayerName(const std::string& name);
    std::string getPlayerName() const;
    void setScore(int score);
    int getScore() const;
    
    // Accessors
    IGraphicsLibrary* getGraphicsLib() const;
    IGameLibrary* getGameLib() const;
    
    // Menu functions
    void displayMenu();
    void displayGameOver();
    void handleMenuInput(int key);
};

}
```

### Core Methods

#### Initialization and Cleanup

- `void initialize()`: Initializes the Core, loading the default or specified libraries.
- `void run()`: Enters the main application loop, handling input, updating game state, and rendering.
- `void cleanup()`: Cleans up resources and unloads libraries.

#### State Management

- `void setState(GameState state)`: Sets the current application state.
- `GameState getState() const`: Gets the current application state.

#### Library Management

- `void loadLibraries()`: Finds and loads available graphics and game libraries.
- `void nextGraphicsLib()`: Switches to the next available graphics library.
- `void nextGameLib()`: Switches to the next available game library.
- `void switchGraphicsLib(const std::string& path)`: Switches to a specific graphics library.
- `void switchGameLib(const std::string& path)`: Switches to a specific game library.

#### Game Data Management

- `void setPlayerName(const std::string& name)`: Sets the current player's name.
- `std::string getPlayerName() const`: Gets the current player's name.
- `void setScore(int score)`: Sets the current game score.
- `int getScore() const`: Gets the current game score.

#### Accessors

- `IGraphicsLibrary* getGraphicsLib() const`: Gets a pointer to the current graphics library.
- `IGameLibrary* getGameLib() const`: Gets a pointer to the current game library.

#### Menu Functions

- `void displayMenu()`: Renders the main menu.
- `void displayGameOver()`: Renders the game over screen.
- `void handleMenuInput(int key)`: Processes input for menu navigation.

## IGraphicsLibrary API

The IGraphicsLibrary interface defines the contract for graphics libraries.

### IGraphicsLibrary Interface

```cpp
namespace arcd {

class IGraphicsLibrary {
public:
    // Key codes
    static const int KEY_UP_CODE = -1;
    static const int KEY_DOWN_CODE = -2;
    static const int KEY_LEFT_CODE = -3;
    static const int KEY_RIGHT_CODE = -4;
    static const int KEY_ENTER_CODE = -5;
    static const int KEY_ESC_CODE = -6;
    static const int KEY_BACKSPACE_CODE = -7;
    static const int KEY_NEXT_LIB_CODE = -8;
    static const int KEY_NEXT_GAME_CODE = -9;
    
    // Colors
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

}
```

### IGraphicsLibrary Methods

#### Initialization and Cleanup

- `bool initialize()`: Initializes the graphics library, setting up windows, resources, etc. Returns true if successful.
- `void cleanup()`: Cleans up resources used by the graphics library.

#### Display Control

- `void clear()`: Clears the screen or display buffer.
- `void refresh()`: Updates the display with current drawing commands.

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

## IGameLibrary API

The IGameLibrary interface defines the contract for game libraries.

### IGameLibrary Interface

```cpp
namespace arcd {

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

}
```

### IGameLibrary Methods

#### Initialization and Cleanup

- `bool initialize()`: Initializes the game, setting up initial state and resources. Returns true if successful.
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

## Dynamic Loading API

The DLLoader template class provides functionality for dynamically loading libraries.

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
    arcd::IGraphicsLibrary* createGraphicsLibrary();
    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* graphicsLib);
}
```

### Game Libraries

```cpp
extern "C" {
    arcd::IGameLibrary* createGameLibrary();
    void destroyGameLibrary(arcd::IGameLibrary* gameLib);
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
float getCurrentTimeSeconds();

// Returns the current time in milliseconds
long long getCurrentTimeMillis();

}
}
``` 