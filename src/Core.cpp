/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** Core
*/

#include "Core.hpp"
#include "utils/Error.hpp"
#include <iostream>

namespace arcd {

Core::Core(const std::string& initialGraphicsLib)
    : _state(AppState::MENU), _selectedMenuOption(0)
{
    _libManager = std::make_unique<LibraryManager>("./lib");
    _scoreManager = std::make_unique<ScoreManager>();
    _playerName = "Player";
    
    // Try to load the initial graphics library
    if (!_libManager->loadGraphicsLibrary(initialGraphicsLib)) {
        throw ArcadeError("Failed to load initial graphics library: " + 
                         _libManager->getLastError());
    }
    
    initializeMenu();
}

Core::~Core() {
    cleanup();
}

bool Core::initialize() {
    // Initialize the graphics library
    auto graphicsLib = _libManager->getCurrentGraphicsLibrary();
    if (!graphicsLib || !graphicsLib->initialize()) {
        displayError("Failed to initialize graphics library");
        return false;
    }
    
    // Scan for available libraries
    _libManager->scanLibraries();
    
    return true;
}

void Core::run() {
    auto graphicsLib = _libManager->getCurrentGraphicsLibrary();
    if (!graphicsLib) {
        return;
    }
    
    while (_state != AppState::EXIT) {
        // Handle input based on current state
        int key = graphicsLib->getKey();
        
        if (_state == AppState::MENU) {
            handleMenuInput(key);
            renderMenu();
        } else if (_state == AppState::GAME) {
            handleGameInput(key);
            renderGame();
        }
        
        graphicsLib->refresh();
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
    // Simple menu navigation
    if (key == 'q') {
        _state = AppState::EXIT;
    } else if (key == 'w') {
        _selectedMenuOption = (_selectedMenuOption > 0) ? _selectedMenuOption - 1 : 0;
    } else if (key == 's') {
        _selectedMenuOption = (_selectedMenuOption < static_cast<int>(_menuOptions.size()) - 1) ? 
                             _selectedMenuOption + 1 : static_cast<int>(_menuOptions.size()) - 1;
    } else if (key == '\n' || key == ' ') {
        // Handle menu selection
        if (_selectedMenuOption == 3) { // Exit option
            _state = AppState::EXIT;
        }
        // Implement other menu options
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
    if (key == 'q') {
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
