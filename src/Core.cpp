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
            switch (_state) {
                case AppState::MENU:
                    handleMenuInput(key);
                    break;
                case AppState::GAME:
                    handleGameInput(key);
                    break;
                case AppState::SELECT_GAME:
                    handleGameSelectionInput(key);
                    break;
                case AppState::SELECT_GRAPHICS:
                    handleGraphicsSelectionInput(key);
                    break;
                default:
                    break;
            }
            
            // Update game state if in game mode
            if (_state == AppState::GAME && _gameManager) {
                _gameManager->update();
            }
            
            graphicsLib->clear();
            
            // Render based on current state
            switch (_state) {
                case AppState::MENU:
                    renderMenu();
                    break;
                case AppState::GAME:
                    renderGame();
                    break;
                case AppState::SELECT_GAME:
                    renderGameSelection();
                    break;
                case AppState::SELECT_GRAPHICS:
                    renderGraphicsSelection();
                    break;
                default:
                    break;
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
    // First, cleanup the graphics library
    if (_libManager) {
        auto graphicsLib = _libManager->getCurrentGraphicsLibrary();
        if (graphicsLib) {
            graphicsLib->cleanup();
        }
        
        // Unload libraries in reverse order of loading
        _libManager->unloadCurrentGameLibrary();
        _libManager->unloadCurrentGraphicsLibrary();
    }
    
    // Reset state
    _state = AppState::EXIT;
    
    // Clear any remaining resources
    _gameManager.reset();
    _libManager.reset();
    _scoreManager.reset();
}

void Core::initializeMenu() {
    _menuOptions = {
        "Select Game",
        "Select Graphics Library",
        "Enter Name",
        "Exit"
    };
    updateLibraryLists();
}

void Core::updateLibraryLists() {
    _gameOptions = _libManager->getGameLibraries();
    _graphicsOptions = _libManager->getGraphicsLibraries();
    _selectedSubMenuOption = 0;
}

void Core::handleMenuInput(int key)
{
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
            updateLibraryLists();
            _state = AppState::SELECT_GAME;
        } else if (_selectedMenuOption == 1) { // Select Graphics Library
            updateLibraryLists();
            _state = AppState::SELECT_GRAPHICS;
        } else if (_selectedMenuOption == 2) { // Enter Name
            auto graphicsLib = _libManager->getCurrentGraphicsLibrary();
            if (graphicsLib) {
                graphicsLib->getPlayerName(_playerName);
            }
        } else if (_selectedMenuOption == 3) { // Exit option
            _state = AppState::EXIT;
        }
    }
}

void Core::handleGameSelectionInput(int key)
{
    if (key == IGraphicsLibrary::KEY_ESC_CODE) {
        _state = AppState::MENU;
    } else if (key == IGraphicsLibrary::KEY_UP_CODE) {
        _selectedSubMenuOption = (_selectedSubMenuOption > 0) ? _selectedSubMenuOption - 1 : 0;
    } else if (key == IGraphicsLibrary::KEY_DOWN_CODE) {
        _selectedSubMenuOption = (_selectedSubMenuOption < static_cast<int>(_gameOptions.size()) - 1) ? 
                                _selectedSubMenuOption + 1 : static_cast<int>(_gameOptions.size()) - 1;
    } else if (key == IGraphicsLibrary::KEY_ENTER_CODE) {
        if (!_gameOptions.empty()) {
            if (_libManager->loadGameLibrary(_gameOptions[_selectedSubMenuOption])) {
                auto gameLib = _libManager->getCurrentGameLibrary();
                if (gameLib && _gameManager->setGame(gameLib)) {
                    gameLib->initialize();
                    _state = AppState::GAME;
                }
            }
        }
    }
}

void Core::handleGraphicsSelectionInput(int key)
{
    if (key == IGraphicsLibrary::KEY_ESC_CODE) {
        _state = AppState::MENU;
    } else if (key == IGraphicsLibrary::KEY_UP_CODE) {
        _selectedSubMenuOption = (_selectedSubMenuOption > 0) ? _selectedSubMenuOption - 1 : 0;
    } else if (key == IGraphicsLibrary::KEY_DOWN_CODE) {
        _selectedSubMenuOption = (_selectedSubMenuOption < static_cast<int>(_graphicsOptions.size()) - 1) ? 
                                _selectedSubMenuOption + 1 : static_cast<int>(_graphicsOptions.size()) - 1;
    } else if (key == IGraphicsLibrary::KEY_ENTER_CODE) {
        if (!_graphicsOptions.empty()) {
            _libManager->loadGraphicsLibrary(_graphicsOptions[_selectedSubMenuOption]);
        }
        _state = AppState::MENU;
    }
}

void Core::renderMenu() {
    auto graphicsLib = _libManager->getCurrentGraphicsLibrary();
    if (!graphicsLib) {
        return;
    }
    
    // Calculate dimensions with safety margins
    int width = std::max(40, graphicsLib->getWidth());
    int height = std::max(20, graphicsLib->getHeight());
    
    // Draw title box with safe dimensions
    int titleWidth = std::min(40, width - 6);  // Leave more margin
    int titleHeight = 3;
    int startX = std::max(2, (width - titleWidth) / 2);
    int startY = std::max(2, (height - titleHeight) / 2);
    
    // Ensure we have enough space and margins
    if (startX < 2 || startY < 2 || startY + titleHeight >= height - 2 || startX + titleWidth >= width - 2) {
        return;
    }
    
    // Draw title
    graphicsLib->drawBox(startX, startY, titleWidth, titleHeight, Color::CYAN);
    std::string title = "ARCADE";
    int titleX = startX + (titleWidth - static_cast<int>(title.length())) / 2;
    graphicsLib->drawText(titleX, startY + 1, title, Color::CYAN);
    
    // Draw menu items with safety checks
    int menuStartY = startY + titleHeight + 2;
    int maxItems = std::min(static_cast<int>(_menuOptions.size()), (height - menuStartY - 2));
    
    for (int i = 0; i < maxItems; i++) {
        std::string prefix = (i == _selectedMenuOption) ? "> " : "  ";
        std::string menuText = prefix + _menuOptions[i];
        if (startX + 2 + static_cast<int>(menuText.length()) < width - 2) {
            graphicsLib->drawText(startX + 2, menuStartY + i, menuText, Color::GREEN);
        }
    }
    
    // Draw side panels with strict bounds checking
    int panelWidth = std::min(20, (width - titleWidth) / 2 - 6);
    if (panelWidth >= 10) {  // Only draw if we have enough space
        int panelX = std::min(width - panelWidth - 2, startX + titleWidth + 4);
        
        // Graphics library panel
        if (startY + 6 < height - 2) {
            graphicsLib->drawBox(panelX, startY, panelWidth, 5, Color::CYAN);
            if (panelX + 2 + 8 < width - 2) {  // Check if text fits
                graphicsLib->drawText(panelX + 2, startY + 1, "Graphics", Color::GREEN);
                std::string name = graphicsLib->getName();
                if (name.length() > static_cast<size_t>(panelWidth - 4)) {
                    name = name.substr(0, panelWidth - 4);
                }
                graphicsLib->drawText(panelX + 2, startY + 2, name, Color::WHITE);
            }
        }
        
        // Game library panel
        if (startY + 12 < height - 2) {
            graphicsLib->drawBox(panelX, startY + 6, panelWidth, 5, Color::CYAN);
            if (panelX + 2 + 4 < width - 2) {  // Check if text fits
                graphicsLib->drawText(panelX + 2, startY + 7, "Game", Color::GREEN);
                auto gameLib = _libManager->getCurrentGameLibrary();
                if (gameLib) {
                    std::string name = gameLib->getName();
                    if (name.length() > static_cast<size_t>(panelWidth - 4)) {
                        name = name.substr(0, panelWidth - 4);
                    }
                    graphicsLib->drawText(panelX + 2, startY + 8, name, Color::WHITE);
                }
            }
        }
    }
    
    // Draw player name with bounds checking
    if (startY > 3) {
        std::string playerInfo = "Player: " + _playerName;
        if (playerInfo.length() > static_cast<size_t>(width - 4)) {
            playerInfo = playerInfo.substr(0, width - 4);
        }
        graphicsLib->drawText(startX + 2, startY - 2, playerInfo, Color::YELLOW);
    }
    
    // Draw controls with bounds checking
    if (height > 8) {
        int controlsY = height - 4;
        std::string controls1 = "Controls: Arrows=Navigate | Enter=Select | ESC=Back";
        std::string controls2 = "R=Restart | Q=Menu | E=Exit";
        
        if (controls1.length() < static_cast<size_t>(width - 4)) {
            graphicsLib->drawText(2, controlsY, controls1, Color::GREEN);
        }
        if (controls2.length() < static_cast<size_t>(width - 4)) {
            graphicsLib->drawText(2, controlsY + 1, controls2, Color::GREEN);
        }
    }
}

void Core::handleGameInput(int key)
{
    if (!_gameManager) {
        _state = AppState::MENU;
        return;
    }

    switch (key) {
        case IGraphicsLibrary::KEY_LEFT_CODE:
            _libManager->loadNextGraphicsLibrary();
            break;
        case IGraphicsLibrary::KEY_RIGHT_CODE:
            _libManager->loadNextGameLibrary();
            break;
        case IGraphicsLibrary::KEY_ESC_CODE:
            _state = AppState::MENU;
            break;
        case 'r':
        case 'R':
            if (_gameManager->isGameOver()) {
                _gameManager->restart();
            }
            break;
        case 'q':
        case 'Q':
            _state = AppState::MENU;
            break;
        case 'e':
        case 'E':
            _state = AppState::EXIT;
            break;
        default:
            _gameManager->handleInput(key);
            break;
    }
}

void Core::renderGame() {
    auto graphicsLib = _libManager->getCurrentGraphicsLibrary();
    if (!graphicsLib || !_gameManager) {
        return;
    }
    
    graphicsLib->clear();
    
    // Let the game render itself using the graphics library
    _gameManager->render(graphicsLib);
    
    // Check if game is over
    if (_gameManager->isGameOver()) {
        // Add score
        _scoreManager->addScore(_playerName, _gameManager->getName(), _gameManager->getScore());
        
        // Return to menu
        _state = AppState::MENU;
    }
}

void Core::renderGameSelection()
{
    auto graphicsLib = _libManager->getCurrentGraphicsLibrary();
    if (!graphicsLib) {
        return;
    }
    
    // Calculate dimensions with safety margins
    int width = std::max(40, graphicsLib->getWidth());
    int height = std::max(20, graphicsLib->getHeight());
    
    // Draw title with safe dimensions
    int titleWidth = std::min(40, width - 6);
    int titleHeight = 3;
    int startX = std::max(2, (width - titleWidth) / 2);
    int startY = std::max(2, (height - titleHeight) / 2);
    
    // Ensure we have enough space and margins
    if (startX < 2 || startY < 2 || startY + titleHeight >= height - 2 || startX + titleWidth >= width - 2) {
        return;
    }
    
    graphicsLib->drawBox(startX, startY - 2, titleWidth, titleHeight, Color::CYAN);
    std::string title = "Select Game";
    int titleX = startX + (titleWidth - static_cast<int>(title.length())) / 2;
    graphicsLib->drawText(titleX, startY - 1, title, Color::CYAN);
    
    // Draw game options with safety checks
    int menuStartY = startY + 2;
    if (_gameOptions.empty()) {
        if (menuStartY < height - 2) {
            graphicsLib->drawText(startX, menuStartY, "No games available", Color::RED);
        }
    } else {
        int maxItems = std::min(static_cast<int>(_gameOptions.size()), (height - menuStartY - 2));
        for (int i = 0; i < maxItems; i++) {
            std::string prefix = (i == _selectedSubMenuOption) ? "> " : "  ";
            std::string menuText = prefix + _gameOptions[i];
            if (startX + static_cast<int>(menuText.length()) < width - 2) {
                graphicsLib->drawText(startX, menuStartY + i, menuText, Color::GREEN);
            }
        }
    }
    
    // Draw controls with bounds checking
    if (height > 4) {
        int controlsY = height - 2;
        std::string controls = "ESC=Back | Enter=Select";
        if (controls.length() < static_cast<size_t>(width - 4)) {
            graphicsLib->drawText(2, controlsY, controls, Color::GREEN);
        }
    }
}

void Core::renderGraphicsSelection()
{
    auto graphicsLib = _libManager->getCurrentGraphicsLibrary();
    if (!graphicsLib) {
        return;
    }
    
    // Calculate dimensions with safety margins
    int width = std::max(40, graphicsLib->getWidth());
    int height = std::max(20, graphicsLib->getHeight());
    
    // Draw title with safe dimensions
    int titleWidth = std::min(40, width - 6);
    int titleHeight = 3;
    int startX = std::max(2, (width - titleWidth) / 2);
    int startY = std::max(2, (height - titleHeight) / 2);
    
    // Ensure we have enough space and margins
    if (startX < 2 || startY < 2 || startY + titleHeight >= height - 2 || startX + titleWidth >= width - 2) {
        return;
    }
    
    graphicsLib->drawBox(startX, startY - 2, titleWidth, titleHeight, Color::CYAN);
    std::string title = "Select Graphics Library";
    int titleX = startX + (titleWidth - static_cast<int>(title.length())) / 2;
    graphicsLib->drawText(titleX, startY - 1, title, Color::CYAN);
    
    // Draw graphics options with safety checks
    int menuStartY = startY + 2;
    if (_graphicsOptions.empty()) {
        if (menuStartY < height - 2) {
            graphicsLib->drawText(startX, menuStartY, "No graphics libraries available", Color::RED);
        }
    } else {
        int maxItems = std::min(static_cast<int>(_graphicsOptions.size()), (height - menuStartY - 2));
        for (int i = 0; i < maxItems; i++) {
            std::string prefix = (i == _selectedSubMenuOption) ? "> " : "  ";
            std::string menuText = prefix + _graphicsOptions[i];
            if (startX + static_cast<int>(menuText.length()) < width - 2) {
                graphicsLib->drawText(startX, menuStartY + i, menuText, Color::GREEN);
            }
        }
    }
    
    // Draw controls with bounds checking
    if (height > 4) {
        int controlsY = height - 2;
        std::string controls = "ESC=Back | Enter=Select";
        if (controls.length() < static_cast<size_t>(width - 4)) {
            graphicsLib->drawText(2, controlsY, controls, Color::GREEN);
        }
    }
}

AppState Core::getState() const {
    return _state;
}

} // namespace arcd
