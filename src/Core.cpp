/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** Core
*/

#include "Core.hpp"
#include "utils/Error.hpp"
#include <iostream>
#include <time.h>

namespace arcd {

Core::Core(const std::string& initialGraphicsLib)
    : _state(AppState::MENU), _selectedMenuOption(0)
{
    _libManager = std::make_unique<LibraryManager>("./lib");
    _scoreManager = std::make_unique<ScoreManager>();
    _gameManager = std::make_unique<GameManager>();
    _playerName = "Player";
    
    if (!_libManager->loadGraphicsLibrary(initialGraphicsLib)) {
        std::string error = "Failed to load initial graphics library: " + _libManager->getLastError();
        throw ArcadeError(error);
    }
    
    initializeMenu();
}

Core::~Core() {
    cleanup();
}

bool Core::initialize()
{
    auto graphicsLib = _libManager->getCurrentGraphicsLibrary();
    if (!graphicsLib) {
        return false;
    }
    
    if (!graphicsLib->initialize()) {
        return false;
    }
    
    return true;
}

void Core::run()
{
    auto graphicsLib = _libManager->getCurrentGraphicsLibrary();
    if (!graphicsLib) {
        return;
    }
    
    // Main loop
    while (_state != AppState::EXIT) {
        try {
            // Process input
            int key = graphicsLib->getKey();
            
            // Check for exit key
            if (key == 'q') {
                _state = AppState::EXIT;
                continue;
            }
            
            // Handle input based on current state
            if (_state == AppState::MENU) {
                handleMenuInput(key);
            } else if (_state == AppState::GAME) {
                handleGameInput(key);
            }
            
            // Update game state if in game mode
            if (_state == AppState::GAME && _gameManager) {
                _gameManager->update();
            }
            
            graphicsLib->clear();
            
            // Render based on current state
            if (_state == AppState::MENU) {
                renderMenu();
            } else if (_state == AppState::GAME) {
                renderGame();
            }
            
            graphicsLib->refresh();
            
        } catch (...) {
            // Silently handle any exceptions
        }
        
        // Add a small delay
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 50000000;
        nanosleep(&ts, NULL);
    }
}

void Core::cleanup() {
    // Cleanup resources
    if (_libManager) {
        auto graphicsLib = _libManager->getCurrentGraphicsLibrary();
        if (graphicsLib) {
            graphicsLib->cleanup();
        }
        
        _libManager->unloadCurrentGraphicsLibrary();
        _libManager->unloadCurrentGameLibrary();
    }
}

void Core::initializeMenu() {
    _menuOptions = {
        "Select Game",
        "Select Graphics Library",
        "Enter Name",
        "Exit"
    };
}

void Core::handleMenuInput(int key) {
    // Updated menu navigation using arrow keys instead of 'w'/'s'
    if (key == 'q' || key == 'Q' || key == 27) { // 'q', 'Q', or ESC key
        _state = AppState::EXIT;
    } else if (key == 259) { // KEY_UP (Arrow Up)
        _selectedMenuOption = (_selectedMenuOption > 0) ? _selectedMenuOption - 1 : 0;
    } else if (key == 258) { // KEY_DOWN (Arrow Down)
        _selectedMenuOption = (_selectedMenuOption < static_cast<int>(_menuOptions.size()) - 1) ? 
                             _selectedMenuOption + 1 : static_cast<int>(_menuOptions.size()) - 1;
    } else if (key == 10) { // KEY_ENTER (Enter key)
        // Handle menu selection
        if (_selectedMenuOption == 0) { // Select Game
            // TODO: Implement game selection
        } else if (_selectedMenuOption == 1) { // Select Graphics Library
            // TODO: Implement graphics library selection
        } else if (_selectedMenuOption == 2) { // Enter Name
            auto graphicsLib = _libManager->getCurrentGraphicsLibrary();
            if (graphicsLib) {
                graphicsLib->getPlayerName(_playerName);
            }
        } else if (_selectedMenuOption == 3) { // Exit option
            _state = AppState::EXIT;
        }
    } else if (key == '9') { // Switch graphics library
        _libManager->switchToNextGraphicsLibrary();
    } else if (key == '7') { // Switch game
        _libManager->switchToNextGameLibrary();
    }
}

void Core::renderMenu() {
    auto graphicsLib = _libManager->getCurrentGraphicsLibrary();
    if (!graphicsLib) {
        return;
    }
    
    graphicsLib->clear();
    
    // Draw title
    graphicsLib->drawText(10, 2, "ARCADE");
    
    // Draw menu options
    for (size_t i = 0; i < _menuOptions.size(); ++i) {
        graphicsLib->drawText(10, 5 + static_cast<int>(i), 
                             (i == static_cast<size_t>(_selectedMenuOption) ? "> " : "  ") + _menuOptions[i]);
    }
    
    // Draw available libraries
    graphicsLib->drawText(40, 2, "Graphics Libraries:");
    auto graphicsLibs = _libManager->getGraphicsLibraries();
    for (size_t i = 0; i < graphicsLibs.size(); ++i) {
        graphicsLib->drawText(40, 3 + i, graphicsLibs[i]);
    }
    
    graphicsLib->drawText(40, 10, "Game Libraries:");
    auto gameLibs = _libManager->getGameLibraries();
    for (size_t i = 0; i < gameLibs.size(); ++i) {
        graphicsLib->drawText(40, 11 + i, gameLibs[i]);
    }
    
    // Draw player name
    graphicsLib->drawText(10, 15, "Player: " + _playerName);
}

void Core::handleGameInput(int key) {
    // Handle game input
    auto gameLib = _libManager->getCurrentGameLibrary();
    if (!gameLib) {
        return;
    }
    
    // Handle special keys for the arcade
    if (key == 'q' || key == 'Q') {
        _state = AppState::MENU;
    } else if (key == 'e' || key == 'E') {
        _state = AppState::EXIT;
    } else if (key == 'r' || key == 'R') {
        // Restart game
        gameLib->restart();
    } else if (key == '9') {
        // Switch graphics library
        _libManager->switchToNextGraphicsLibrary();
    } else if (key == '7') {
        // Return to menu to switch games
        _state = AppState::MENU;
    } else {
        // Pass other keys to the game
        gameLib->handleInput(key);
    }
}

void Core::renderGame() {
    auto graphicsLib = _libManager->getCurrentGraphicsLibrary();
    auto gameLib = _libManager->getCurrentGameLibrary();
    
    if (!graphicsLib || !gameLib) {
        return;
    }
    
    graphicsLib->clear();
    
    // Let the game render itself using the graphics library
    gameLib->render(graphicsLib);
    
    // Check if game is over
    if (gameLib->isGameOver()) {
        // Add score
        _scoreManager->addScore(_playerName, gameLib->getName(), gameLib->getScore());
        
        // Return to menu
        _state = AppState::MENU;
    }
}

void Core::displayError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
}

AppState Core::getState() const {
    return _state;
}

} // namespace arcd
