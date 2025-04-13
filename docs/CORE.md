# Core Engine Documentation

The Core Engine is the central component of the Arcade project, responsible for managing the application lifecycle, loading and coordinating the graphics and game libraries, and facilitating communication between them.

## Responsibilities

The Core Engine handles:

1. **Dynamic Library Management**: Loading, initializing, and switching between graphics and game libraries.
2. **Game Loop**: Running the main application loop, managing timing, and coordinating updates.
3. **Input Routing**: Passing user inputs from the graphics library to the game.
4. **Rendering Coordination**: Enabling games to send drawing commands to the graphics library.
5. **State Management**: Tracking application state (menu, gameplay, game over, etc.).
6. **Error Handling**: Recovering from library errors when possible.

## Core Class Structure

### `Core` Class

The main class managing the entire application, found in `include/Core.hpp` and implemented in `src/core/Core.cpp`.

```cpp
class Core {
private:
    // Library management
    std::unique_ptr<DLLoader<IGraphicsLibrary>> _graphicsLoader;
    std::unique_ptr<DLLoader<IGameLibrary>> _gameLoader;
    IGraphicsLibrary* _graphicsLib;
    IGameLibrary* _gameLib;
    
    // Available libraries
    std::vector<std::string> _graphicsLibPaths;
    std::vector<std::string> _gameLibPaths;
    
    // State variables
    bool _running;
    GameState _state;
    
    // Game-specific data
    std::string _playerName;
    int _score;
    
    // Helper methods for library management
    void loadLibraries();
    void switchGraphicsLib(const std::string& path);
    void switchGameLib(const std::string& path);
    
public:
    Core();
    ~Core();
    
    // Main methods
    void initialize();
    void run();
    void cleanup();
    
    // State management
    void setState(GameState state);
    GameState getState() const;
    
    // Library switching
    void nextGraphicsLib();
    void nextGameLib();
    
    // Game management
    void setPlayerName(const std::string& name);
    void setScore(int score);
    int getScore() const;
    std::string getPlayerName() const;
    
    // Graphics access
    IGraphicsLibrary* getGraphicsLib() const;
};
```

### `DLLoader` Class

A templated utility class for loading dynamic libraries.

```cpp
template<typename T>
class DLLoader {
private:
    void* _handle;
    T* _instance;
    std::string _libPath;
    
    // Function pointers to library entry points
    T* (*_createFunc)();
    void (*_destroyFunc)(T*);
    
public:
    DLLoader();
    ~DLLoader();
    
    bool load(const std::string& path);
    void unload();
    
    T* getInstance() const;
    std::string getPath() const;
};
```

## Library Management

### Loading Libraries

The Core searches for libraries in the `lib/` directory with specific naming conventions:
- Graphics libraries: `arcade_*Graphics.so`
- Game libraries: `arcade_*Game.so`

When loading a library:
1. The `DLLoader` opens the library using `dlopen()`
2. It finds the required entry points using `dlsym()`
3. It calls the creation function to get an instance of the library

### Switching Libraries

To switch libraries at runtime:
1. The Core unloads the current library
2. It loads the new library
3. It initializes the new library with current application state
4. The application continues with the new library

## Main Loop

The Core's main loop performs these operations:

```cpp
void Core::run() {
    while (_running) {
        // Process input
        int key = _graphicsLib->getKey();
        handleInput(key);
        
        // Update game state (if in gameplay mode)
        if (_state == GameState::PLAYING) {
            _gameLib->update(this);
        }
        
        // Render current frame
        _graphicsLib->clear();
        
        if (_state == GameState::MENU) {
            displayMenu();
        } else if (_state == GameState::PLAYING) {
            _gameLib->render(_graphicsLib);
        } else if (_state == GameState::GAME_OVER) {
            displayGameOver();
        }
        
        _graphicsLib->refresh();
    }
}
```

## State Management

The Core manages these primary states:

- **MENU**: Displaying the main menu
- **PLAYING**: Active gameplay
- **GAME_OVER**: Game over screen
- **HIGH_SCORES**: High score display

State transitions are triggered by:
- User input in menus
- Game events (like player death)
- System commands (like switching games)

## Error Handling

The Core implements robust error handling:

- Library loading errors are caught and handled gracefully
- If a library fails to initialize, the Core attempts to fall back to another
- Runtime errors in libraries are contained and logged
- Critical errors show clear error messages and exit cleanly

## Next Steps

For more information on components that interact with the Core:
- [Graphics Libraries Documentation](GRAPHICS_LIBS.md)
- [Game Libraries Documentation](GAME_LIBS.md)
- [Interfaces Documentation](INTERFACES.md) 