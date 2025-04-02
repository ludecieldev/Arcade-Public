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
#include <ncurses.h>
#include <string.h>

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
    if (key == IGraphicsLibrary::KEY_ESC_CODE) {
        _state = AppState::EXIT;
    } else if (key == IGraphicsLibrary::KEY_UP_CODE) {
        _selectedMenuOption = (_selectedMenuOption > 0) ? _selectedMenuOption - 1 : 0;
    } else if (key == IGraphicsLibrary::KEY_DOWN_CODE) {
        _selectedMenuOption = (_selectedMenuOption < static_cast<int>(_menuOptions.size()) - 1) ? 
                             _selectedMenuOption + 1 : static_cast<int>(_menuOptions.size()) - 1;
    } else if (key == IGraphicsLibrary::KEY_ENTER_CODE) {
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
    
    // Draw title box
    int titleWidth = 40;
    int titleHeight = 3;
    int startX = (graphicsLib->getWidth() - titleWidth) / 2;
    int startY = (graphicsLib->getHeight() - titleHeight) / 2;
    
    graphicsLib->drawBox(startX, startY, titleWidth, titleHeight, Color::CYAN);
    graphicsLib->drawText(startX + (titleWidth - 6) / 2, startY + 1, "ARCADE", Color::CYAN);
    
    // Draw menu items
    int menuStartY = startY + titleHeight + 2;
    for (size_t i = 0; i < _menuOptions.size(); i++) {
        std::string prefix = (i == static_cast<size_t>(_selectedMenuOption)) ? "> " : "  ";
        graphicsLib->drawText(startX + 2, menuStartY + i, prefix + _menuOptions[i], Color::GREEN);
    }
    
    // Draw side panels
    int panelWidth = 20;
    int panelX = startX + titleWidth + 4;
    
    // Graphics library panel
    graphicsLib->drawBox(panelX, startY, panelWidth, 5, Color::CYAN);
    graphicsLib->drawText(panelX + 2, startY + 1, "Graphics Library", Color::GREEN);
    graphicsLib->drawText(panelX + 2, startY + 2, graphicsLib->getName(), Color::WHITE);
    
    // Game library panel
    graphicsLib->drawBox(panelX, startY + 6, panelWidth, 5, Color::CYAN);
    graphicsLib->drawText(panelX + 2, startY + 7, "Game Library", Color::GREEN);
    auto gameLib = _libManager->getCurrentGameLibrary();
    if (gameLib) {
        graphicsLib->drawText(panelX + 2, startY + 8, gameLib->getName(), Color::WHITE);
    }
    
    // Draw player name
    std::string playerInfo = "Player: " + _playerName;
    graphicsLib->drawText(startX + 2, startY - 2, playerInfo, Color::YELLOW);
    
    // Draw controls
    int controlsY = graphicsLib->getHeight() - 4;
    graphicsLib->drawText(2, controlsY, "Controls: Arrow Keys=Navigate | Enter=Select | 9=Next Lib | 7=Next Game", Color::GREEN);
    graphicsLib->drawText(2, controlsY + 1, "R=Restart | Q=Menu | E=Exit", Color::GREEN);
    
    graphicsLib->refresh();
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

AppState Core::getState() const {
    return _state;
}

} // namespace arcd
