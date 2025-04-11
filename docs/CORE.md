# Core Engine Documentation

The Core Engine is the central component of the Arcade project, responsible for managing the application lifecycle, loading and coordinating the graphics and game libraries, and facilitating communication between them.

## Responsibilities

The Core Engine handles:

1. **Dynamic Library Management**: Loading, initializing, and switching between graphics and game libraries.
2. **Game Loop**: Running the main application loop, managing timing, and coordinating updates.
3. **State Management**: Managing different application states (menu, game selection, gameplay, etc.).
4. **Event Coordination**: Routing events from graphics libraries to games or handling them internally.
5. **UI Management**: Generating and providing UI elements to graphics libraries.
6. **Error Handling**: Recovering from library errors when possible.

## Core Architecture

The Core follows the mediator design pattern, serving as the central hub that:
- Maintains separation between graphics libraries and games
- Ensures neither directly depends on the other
- Translates generic states and events between components

## Main Components

### Core Class

The main controller class that manages everything:

```cpp
class Core : public ICore {
private:
    std::unique_ptr<LibraryManager> _libManager;
    std::unique_ptr<ScoreManager> _scoreManager;
    std::unique_ptr<GameManager> _gameManager;
    CoreState _state;
    std::string _playerName;
    
    // Menu state
    int _selectedMenuOption;
    int _selectedSubMenuOption;
    std::vector<std::string> _menuOptions;
    std::vector<std::string> _gameOptions;
    std::vector<std::string> _graphicsOptions;
    
    // Timing variables
    std::chrono::high_resolution_clock::time_point _lastFrameTime;
    double _deltaTime;
    
    // Game state cache
    std::unique_ptr<IGameState> _currentGameState;
    
    // UI Elements
    std::vector<UIElement> _uiElements;
    
    // Private methods
    void initializeMenu();
    void updateLibraryLists();
    void updateGameState();
    void updateUI();
    void calculateDeltaTime();
    void askPlayerName();
    
    // Event handling
    void handleMenuEvent(const IEvent& event);
    void handleGameEvent(const IEvent& event);
    void handleGlobalEvent(const IEvent& event);
    void handleSelectGameEvent(const IEvent& event);
    void handleSelectGraphicsEvent(const IEvent& event);

public:
    Core(const std::string& initialGraphicsLib);
    ~Core() override;

    // Core lifecycle
    bool initialize() override;
    void run() override;
    void cleanup() override;
    
    // Event handling
    void processEvents() override;

    // Library management
    bool loadGameLibrary(const std::string& path) override;
    bool loadGraphicsLibrary(const std::string& path) override;
    bool switchGameLibrary(const std::string& name) override;
    bool switchGraphicsLibrary(const std::string& name) override;
    
    // Resource access
    std::vector<std::string> getAvailableGameLibraries() const override;
    std::vector<std::string> getAvailableGraphicsLibraries() const override;
    CoreState getState() const override;
    void setState(CoreState state) override;
    const IGameState* getCurrentGameState() const override;
    std::vector<UIElement> getUIElements() const override;
    
    // Player management
    void setPlayerName(const std::string& name) override;
    std::string getPlayerName() const override;
    
    // Score management
    void saveScore(const std::string& gameName, int score) override;
    std::vector<std::pair<std::string, int>> getHighScores(const std::string& gameName) const override;
};
```

### LibraryManager

Handles the loading and management of dynamic libraries:

```cpp
class LibraryManager {
private:
    std::string _libDirectory;
    std::unique_ptr<ILibraryLoader> _graphicsLoader;
    std::unique_ptr<ILibraryLoader> _gameLoader;
    
    std::map<std::string, std::string> _graphicsLibs;
    std::map<std::string, std::string> _gameLibs;
    
    std::unique_ptr<IGraphicsLibrary> _currentGraphicsLib;
    std::unique_ptr<IGameLibrary> _currentGameLib;
    
    size_t _currentGraphicsIndex;
    size_t _currentGameIndex;
    
public:
    LibraryManager(const std::string& libDirectory);
    ~LibraryManager();
    
    // Library discovery
    void scanLibraries();
    std::vector<std::string> getGraphicsLibraries() const;
    std::vector<std::string> getGameLibraries() const;
    
    // Library loading
    bool loadGraphicsLibrary(const std::string& path);
    bool loadGameLibrary(const std::string& path);
    void unloadCurrentGraphicsLibrary();
    void unloadCurrentGameLibrary();
    
    // Library access
    IGraphicsLibrary& getCurrentGraphicsLibrary();
    IGameLibrary& getCurrentGameLibrary();
    std::string getCurrentGraphicsLibraryName() const;
    std::string getCurrentGameLibraryName() const;
    
    // Library status
    bool hasGraphicsLibrary() const;
    bool hasGameLibrary() const;
};
```

### GameManager

Manages the currently active game:

```cpp
class GameManager {
private:
    std::unique_ptr<IGameLibrary> _currentGame;
    bool _isPaused;
    
public:
    GameManager();
    ~GameManager();
    
    // Game management
    bool setGame(std::unique_ptr<IGameLibrary> game);
    IGameLibrary& getCurrentGame();
    bool hasGame() const;
    void resetGame();
    
    // Game state
    void pauseGame();
    void resumeGame();
    bool isPaused() const;
    
    // Game loop
    void update(double deltaTime);
    
    // Event handling
    void processEvent(const IEvent& event);
    
    // Game state access
    std::unique_ptr<IGameState> getGameState() const;
    bool isGameOver() const;
    int getScore() const;
    std::string getName() const;
    std::string getDescription() const;
};
```

## Core States

The Core manages the following states:
- **MENU**: Main menu for selecting actions
- **GAME**: Active gameplay
- **SELECT_GAME**: Interface for choosing a game
- **SELECT_GRAPHICS**: Interface for choosing a graphics library
- **GAME_OVER**: Displaying the game over screen
- **PAUSE**: Game is paused
- **EXIT**: Preparing to exit the application

## Main Game Loop

The Core's main loop handles:

1. **Delta Time Calculation**: To ensure frame-rate independent updates
2. **Event Processing**: Capturing input events from the graphics library
3. **State Updates**: Updating game state based on current mode 
4. **UI Generation**: Creating UI elements based on current state
5. **Rendering**: Instructing the graphics library to render the game state and UI

```cpp
void Core::run()
{
    // Main loop
    while (_state != CoreState::EXIT) {
        // Calculate delta time
        calculateDeltaTime();
        
        // Process input events
        processEvents();
        
        // Update game state if in game mode
        if (_state == CoreState::GAME) {
            if (_libManager->hasGameLibrary() && _gameManager) {
                _gameManager->update(_deltaTime);
                updateGameState();
            } else {
                std::cerr << "Error: Missing game library in GAME state" << std::endl;
                _state = CoreState::MENU;
            }
        }
        
        // Update UI elements
        updateUI();
        
        // Clear screen and render
        auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
        graphicsLib.clear();
        graphicsLib.renderUI(_uiElements);
        
        // Render game state if available
        if (_state == CoreState::GAME && _currentGameState) {
            graphicsLib.renderGameState(*_currentGameState);
        }
        
        // Refresh display
        graphicsLib.refresh();
    }
}
```

## Event Handling

The Core processes events based on the current state:

```cpp
void Core::processEvents()
{
    auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
    
    auto eventOpt = graphicsLib.pollEvent();
    if (!eventOpt) {
        return;
    }
    
    auto& event = *eventOpt.value();
    
    // Global events handling (applicable in any state)
    handleGlobalEvent(event);
    
    // State-specific event handling
    switch (_state) {
        case CoreState::MENU:
            handleMenuEvent(event);
            break;
        case CoreState::GAME:
            handleGameEvent(event);
            break;
        case CoreState::SELECT_GAME:
            handleSelectGameEvent(event);
            break;
        case CoreState::SELECT_GRAPHICS:
            handleSelectGraphicsEvent(event);
            break;
        // Other states...
    }
}
```

## Library Management

The Core manages library loading and switching:

1. **Initial Loading**: On startup, loads specified graphics and game libraries
2. **Discovery**: Scans the library directory for available libraries
3. **Switching**: Handles runtime switching between libraries
4. **Error Recovery**: Attempts to recover from failed library loads

## Best Practices for Extending the Core

When modifying or extending the Core:

1. **Maintain Separation**: Keep graphics libraries and games completely separate
2. **Use Interfaces**: Always communicate through interfaces, not specific implementations
3. **State Management**: Clearly define state transitions and handlers
4. **Error Handling**: Implement robust error handling to prevent crashes
5. **Resource Management**: Use smart pointers and RAII for resources
6. **Clean API**: Provide a clean, consistent API for libraries to use

## Next Steps

For more information on components that interact with the Core:
- [Graphics Libraries Documentation](GRAPHICS_LIBS.md)
- [Game Libraries Documentation](GAME_LIBS.md)
- [Interfaces Documentation](INTERFACES.md) 