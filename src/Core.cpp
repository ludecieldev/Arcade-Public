/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** Core
*/

#include "Core.hpp"
#include "utils/Error.hpp"
#include "utils/Event.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>

namespace arcd {

Core::Core(const std::string& initialGraphicsLib)
    : _state(CoreState::MENU), _selectedMenuOption(0), _deltaTime(0.0)
{
    _libManager = std::make_unique<LibraryManager>("./lib");
    _scoreManager = std::make_unique<ScoreManager>();
    _gameManager = std::make_unique<GameManager>();
    _playerName = "Player";
    
    // Initialize time tracking
    _lastFrameTime = std::chrono::high_resolution_clock::now();
    
    // Scan for available libraries
    _libManager->scanLibraries();
    
    // Load initial graphics library
    if (!_libManager->loadGraphicsLibrary(initialGraphicsLib)) {
        std::string error = "Failed to load initial graphics library: " + _libManager->getLastError();
        throw ArcadeError(error);
    }
    
    // Load first available game library
    auto gameLibs = _libManager->getGameLibraries();
    if (!gameLibs.empty()) {
        if (!_libManager->loadGameLibrary(gameLibs[0])) {
            std::cerr << "Warning: Failed to load initial game library: " << _libManager->getLastError() << std::endl;
        }
    }
    
    initializeMenu();
}

Core::~Core() {
    cleanup();
}

bool Core::initialize()
{
    if (!_libManager->hasGraphicsLibrary()) {
        return false;
    }
    
    try {
        auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
        return graphicsLib.initialize();
    } catch (const std::exception& e) {
        std::cerr << "Initialization error: " << e.what() << std::endl;
        return false;
    }
}

void Core::calculateDeltaTime()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    _deltaTime = std::chrono::duration<double>(currentTime - _lastFrameTime).count();
    _lastFrameTime = currentTime;
}

void Core::run()
{
    if (!_libManager->hasGraphicsLibrary()) {
        std::cerr << "Error: No graphics library loaded" << std::endl;
        _state = CoreState::EXIT;
        return;
    }
    
    // Main loop
    while (_state != CoreState::EXIT) {
        try {
            // Calculate delta time for frame-rate independent updates
            calculateDeltaTime();
            
            // Verify that the graphics library is still valid
            if (!_libManager->hasGraphicsLibrary()) {
                std::cerr << "Error: Graphics library was unloaded unexpectedly" << std::endl;
                _state = CoreState::EXIT;
                break;
            }
            
            auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
            
            // Process input events
            processGameEvents();
            
            // Update game state if in game mode
            if (_state == CoreState::GAME && _gameManager && _gameManager->hasGame()) {
                _gameManager->update(_deltaTime);
                updateGameState();
            }
            
            // Update UI elements based on current state
            updateUI();
            
            // Clear screen for new frame
            graphicsLib.clear();
            
            // Render UI elements (menus, etc.)
            graphicsLib.renderUI(_uiElements);
            
            // Render game state if available
            if (_state == CoreState::GAME && _currentGameState) {
                graphicsLib.renderGameState(*_currentGameState);
            }
            
            // Refresh display
            graphicsLib.refresh();
            
            // Small delay to avoid consuming too much CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60fps cap
            
        } catch (const std::exception& e) {
            std::cerr << "Error in main loop: " << e.what() << std::endl;
            // Short pause to avoid spamming errors
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        } catch (...) {
            std::cerr << "Unknown error in main loop" << std::endl;
            // Short pause to avoid spamming errors
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}

void Core::processGameEvents()
{
    if (!_libManager->hasGraphicsLibrary()) {
        return;
    }
    
    auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
    
    // Process all pending events
    while (auto eventOpt = graphicsLib.pollEvent()) {
        auto& event = *eventOpt;
        
        // Process event based on current state
        processEvent(*event);
    }
}

void Core::processEvent(const IEvent& event)
{
    // First, handle global events (like quitting)
    handleGlobalEvent(event);
    
    // Then handle state-specific events
    switch (_state) {
        case CoreState::MENU:
            handleMenuEvent(event);
            break;
        case CoreState::GAME:
            handleGameEvent(event);
            break;
        default:
            break;
    }
}

void Core::handleGlobalEvent(const IEvent& event)
{
    // Check for quit event
    if (event.getType() == EventType::KEY_PRESSED) {
        if (event.getKeyCode() == KeyCode::QUIT) {
            _state = CoreState::EXIT;
        } else if (event.getKeyCode() == KeyCode::NEXT_LIB) {
            // Switch to next graphics library
            auto libs = _libManager->getGraphicsLibraries();
            if (libs.size() > 1) {
                std::string currentLib = _libManager->getCurrentGraphicsLibraryName();
                auto it = std::find(libs.begin(), libs.end(), currentLib);
                if (it != libs.end()) {
                    ++it;
                    if (it == libs.end()) {
                        it = libs.begin();
                    }
                    switchGraphicsLibrary(*it);
                }
            }
        } else if (event.getKeyCode() == KeyCode::NEXT_GAME) {
            // Switch to next game library
            auto games = _libManager->getGameLibraries();
            if (games.size() > 1) {
                std::string currentGame = _gameManager->getName();
                auto it = std::find_if(games.begin(), games.end(), 
                    [&](const std::string& game) {
                        return game.find(currentGame) != std::string::npos;
                    });
                if (it != games.end()) {
                    ++it;
                    if (it == games.end()) {
                        it = games.begin();
                    }
                    switchGameLibrary(*it);
                }
            }
        }
    }
}

void Core::handleMenuEvent(const IEvent& event)
{
    if (event.getType() != EventType::KEY_PRESSED) {
        return;
    }
    
    switch (event.getKeyCode()) {
        case KeyCode::UP:
            _selectedMenuOption = (_selectedMenuOption > 0) ? _selectedMenuOption - 1 : 0;
            break;
        case KeyCode::DOWN:
            _selectedMenuOption = (_selectedMenuOption < static_cast<int>(_menuOptions.size()) - 1) ? 
                _selectedMenuOption + 1 : _menuOptions.size() - 1;
            break;
        case KeyCode::ENTER:
            // Handle menu selection
            switch (_selectedMenuOption) {
                case 0: // Play Game
                    if (_libManager->hasGameLibrary()) {
                        _state = CoreState::GAME;
                        if (_gameManager) {
                            _gameManager->resetGame();
                        }
                    }
                    break;
                case 1: // Select Game
                    updateLibraryLists();
                    _selectedSubMenuOption = 0;
                    _state = CoreState::SELECT_GAME;
                    break;
                case 2: // Select Graphics
                    updateLibraryLists();
                    _selectedSubMenuOption = 0;
                    _state = CoreState::SELECT_GRAPHICS;
                    break;
                case 3: // Leaderboard
                    // TODO: Implement leaderboard view
                    break;
                case 4: // Exit
                    _state = CoreState::EXIT;
                    break;
            }
            break;
    }
}

void Core::handleGameEvent(const IEvent& event)
{
    // First check for back to menu
    if (event.getType() == EventType::KEY_PRESSED && event.getKeyCode() == KeyCode::ESC) {
        _state = CoreState::MENU;
        return;
    }
    
    // Pass event to game if we have one
    if (_gameManager && _gameManager->hasGame()) {
        // Let the game process the event
        bool handled = _gameManager->processEvent(event);
        
        // If game is over, save score
        if (_gameManager->isGameOver()) {
            saveScore(_gameManager->getName(), _gameManager->getScore());
        }
    }
}

void Core::updateUI()
{
    _uiElements.clear();
    
    // Prepare different UI based on current state
    switch (_state) {
        case CoreState::MENU:
            // Title
            _uiElements.push_back({
                UIElementType::TEXT,
                (_libManager->getCurrentGraphicsLibrary().getWidth() - 6) / 2,
                2,
                6, 1,
                "ARCADE",
                Color::CYAN,
                false
            });
            
            // Menu options
            for (size_t i = 0; i < _menuOptions.size(); i++) {
                _uiElements.push_back({
                    UIElementType::TEXT,
                    (_libManager->getCurrentGraphicsLibrary().getWidth() - _menuOptions[i].length()) / 2,
                    6 + static_cast<int>(i) * 2,
                    static_cast<int>(_menuOptions[i].length()), 1,
                    _menuOptions[i],
                    Color::WHITE,
                    static_cast<int>(i) == _selectedMenuOption
                });
            }
            
            // Current graphics and game info
            std::string graphicsInfo = "Graphics: " + _libManager->getCurrentGraphicsLibraryName();
            std::string gameInfo = "Game: " + (_gameManager->hasGame() ? _gameManager->getName() : "None");
            
            _uiElements.push_back({
                UIElementType::TEXT,
                2, _libManager->getCurrentGraphicsLibrary().getHeight() - 2,
                static_cast<int>(graphicsInfo.length()), 1,
                graphicsInfo,
                Color::YELLOW,
                false
            });
            
            _uiElements.push_back({
                UIElementType::TEXT,
                2, _libManager->getCurrentGraphicsLibrary().getHeight() - 3,
                static_cast<int>(gameInfo.length()), 1,
                gameInfo,
                Color::YELLOW,
                false
            });
            
            break;
            
        case CoreState::GAME:
            // Game state is rendered separately in renderGameState
            // Here we just add UI overlays like score
            if (_gameManager->hasGame()) {
                std::string scoreText = "Score: " + std::to_string(_gameManager->getScore());
                _uiElements.push_back({
                    UIElementType::TEXT,
                    2, 1,
                    static_cast<int>(scoreText.length()), 1,
                    scoreText,
                    Color::YELLOW,
                    false
                });
                
                // Help text
                std::string helpText = "ESC: Menu | 7: Next Game | 9: Next Graphics";
                _uiElements.push_back({
                    UIElementType::TEXT,
                    (_libManager->getCurrentGraphicsLibrary().getWidth() - helpText.length()) / 2,
                    _libManager->getCurrentGraphicsLibrary().getHeight() - 1,
                    static_cast<int>(helpText.length()), 1,
                    helpText,
                    Color::WHITE,
                    false
                });
            }
            break;
            
        default:
            break;
    }
}

void Core::updateGameState()
{
    if (_gameManager && _gameManager->hasGame()) {
        _currentGameState = _gameManager->getGameState();
    }
}

void Core::cleanup() {
    // First, cleanup the graphics library
    if (_libManager && _libManager->hasGraphicsLibrary()) {
        try {
            auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
            graphicsLib.cleanup();
            
            // Unload libraries in reverse order of loading
            _libManager->unloadCurrentGameLibrary();
            _libManager->unloadCurrentGraphicsLibrary();
        } catch (...) {
            // Silently ignore cleanup errors
        }
    }
    
    // Reset state
    _state = CoreState::EXIT;
    
    // Clear any remaining resources
    _gameManager.reset();
    _libManager.reset();
    _scoreManager.reset();
}

void Core::initializeMenu() {
    _menuOptions = {
        "Play Game",
        "Select Game",
        "Select Graphics",
        "Leaderboard",
        "Exit"
    };
    _selectedMenuOption = 0;
}

void Core::updateLibraryLists() {
    _gameOptions = _libManager->getGameLibraries();
    _graphicsOptions = _libManager->getGraphicsLibraries();
}

// ICore implementation methods

CoreState Core::getState() const {
    return _state;
}

void Core::setState(CoreState state) {
    _state = state;
}

bool Core::loadGameLibrary(const std::string& path) {
    return _libManager->loadGameLibrary(path);
}

bool Core::loadGraphicsLibrary(const std::string& path) {
    return _libManager->loadGraphicsLibrary(path);
}

bool Core::switchGameLibrary(const std::string& name) {
    if (_libManager->loadGameLibrary(name)) {
        if (_gameManager) {
            _gameManager->resetGame();
        }
        return true;
    }
    return false;
}

bool Core::switchGraphicsLibrary(const std::string& name) {
    return _libManager->loadGraphicsLibrary(name);
}

std::vector<std::string> Core::getAvailableGameLibraries() const {
    return _libManager->getGameLibraries();
}

std::vector<std::string> Core::getAvailableGraphicsLibraries() const {
    return _libManager->getGraphicsLibraries();
}

const IGameState* Core::getCurrentGameState() const {
    return _currentGameState.get();
}

std::vector<UIElement> Core::getUIElements() const {
    return _uiElements;
}

void Core::setPlayerName(const std::string& name) {
    _playerName = name;
}

std::string Core::getPlayerName() const {
    return _playerName;
}

void Core::saveScore(const std::string& gameName, int score) {
    _scoreManager->addScore(gameName, _playerName, score);
}

std::vector<std::pair<std::string, int>> Core::getHighScores(const std::string& gameName) const {
    return _scoreManager->getTopScores(gameName);
}

} // namespace arcd
