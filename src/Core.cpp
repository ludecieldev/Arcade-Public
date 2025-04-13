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
    : _state(AppState::MENU), 
      _selectedMenuOption(0), 
      _selectedSubMenuOption(0),
      _selectedGameIndex(0),
      _selectedGraphicsIndex(0)
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

void Core::run()
{
    if (!_libManager->hasGraphicsLibrary()) {
        std::cerr << "Error: No graphics library loaded" << std::endl;
        _state = AppState::EXIT;
        return;
    }
    
    // If we have a game library loaded at start, initialize the game manager
    if (_libManager->hasGameLibrary()) {
        _gameManager->initializeFromLibraryManager(*_libManager);
    }
    
    // Main loop
    while (_state != AppState::EXIT) {
        try {
            // Vérifier que la bibliothèque graphique est toujours valide
            if (!_libManager->hasGraphicsLibrary()) {
                std::cerr << "Error: Graphics library was unloaded unexpectedly" << std::endl;
                _state = AppState::EXIT;
                break;
            }
            
            auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
            
            // Process keyboard input
            int key = graphicsLib.getKey();
            
            // Process mouse input (sera utilisé plus tard par Minesweeper)
            IGraphicsLibrary::MouseEvent mouseEvent = graphicsLib.getMouse();
            if (mouseEvent.hasEvent && _state == AppState::GAME && _libManager->hasGameLibrary()) {
                auto& game = _libManager->getCurrentGameLibrary();
                game.handleMouseInput(mouseEvent);
            }
            
            // Check for exit key
            if (key == 'q') {
                _state = AppState::EXIT;
                continue;
            }
            
            // Handle keyboard input based on current state
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
            
            // Vérifier à nouveau que la bibliothèque est valide après le traitement des entrées
            if (!_libManager->hasGraphicsLibrary()) {
                std::cerr << "Error: Graphics library was unloaded during input handling" << std::endl;
                _state = AppState::EXIT;
                break;
            }
            
            // Update game state if in game mode
            if (_state == AppState::GAME && _libManager->hasGameLibrary()) {
                try {
                    auto& game = _libManager->getCurrentGameLibrary();
                    game.update();
                } catch (const std::exception& e) {
                    std::cerr << "Error updating game: " << e.what() << std::endl;
                }
            }
            
            // Retenir une référence à la bibliothèque graphique pour cette itération
            auto& currentGraphicsLib = _libManager->getCurrentGraphicsLibrary();
            currentGraphicsLib.clear();
            
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
            
            // Vérifier encore une fois que la bibliothèque est valide après le rendu
            if (!_libManager->hasGraphicsLibrary()) {
                std::cerr << "Error: Graphics library was unloaded during rendering" << std::endl;
                _state = AppState::EXIT;
                break;
            }
            
            currentGraphicsLib.refresh();
        } catch (const std::exception& e) {
            std::cerr << "Error in main loop: " << e.what() << std::endl;
            // Une courte pause pour éviter de spammer les erreurs
            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 500000000; // 500ms
            nanosleep(&ts, NULL);
        } catch (...) {
            std::cerr << "Unknown error in main loop" << std::endl;
            // Une courte pause pour éviter de spammer les erreurs
            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 500000000; // 500ms
            nanosleep(&ts, NULL);
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
    
    // Mettre à jour les indices sélectionnés
    _selectedSubMenuOption = 0;
    
    // Trouver l'index de la bibliothèque de jeu courante
    _selectedGameIndex = 0;
    if (_libManager->hasGameLibrary()) {
        std::string currentGame = _libManager->getCurrentGameLibraryName();
        for (size_t i = 0; i < _gameOptions.size(); i++) {
            if (_gameOptions[i] == currentGame) {
                _selectedGameIndex = static_cast<int>(i);
                break;
            }
        }
    }
    
    // Trouver l'index de la bibliothèque graphique courante
    _selectedGraphicsIndex = 0;
    if (_libManager->hasGraphicsLibrary()) {
        std::string currentGraphics = _libManager->getCurrentGraphicsLibraryName();
        for (size_t i = 0; i < _graphicsOptions.size(); i++) {
            if (_graphicsOptions[i] == currentGraphics) {
                _selectedGraphicsIndex = static_cast<int>(i);
                break;
            }
        }
    }
}

void Core::handleMenuInput(int key)
{
    if (key == IGraphicsLibrary::KEY_ESC_CODE) {
        _state = AppState::EXIT;
        return;
    }
    
    // TAB pour naviguer entre les boîtes
    if (key == IGraphicsLibrary::KEY_TAB_CODE) {
        _selectedMenuOption = (_selectedMenuOption + 1) % 3; // 3 boîtes: jeux, graphiques, joueur
        return;
    }
    
    // Utiliser les flèches selon la boîte sélectionnée
    if (_selectedMenuOption == 0) { // Boîte des jeux
        if (key == IGraphicsLibrary::KEY_UP_CODE) {
            if (_gameOptions.size() > 0) {
                _selectedGameIndex = (_selectedGameIndex > 0) ? _selectedGameIndex - 1 : 0;
            }
        } else if (key == IGraphicsLibrary::KEY_DOWN_CODE) {
            if (_gameOptions.size() > 0) {
                _selectedGameIndex = (_selectedGameIndex < static_cast<int>(_gameOptions.size()) - 1) ? 
                                   _selectedGameIndex + 1 : static_cast<int>(_gameOptions.size()) - 1;
            }
        } else if (key == IGraphicsLibrary::KEY_ENTER_CODE) {
            // Charger la bibliothèque sélectionnée uniquement lors de l'appui sur ENTER
            if (_gameOptions.size() > 0 && _selectedGameIndex >= 0 && _selectedGameIndex < static_cast<int>(_gameOptions.size())) {
                if (_libManager->loadGameLibrary(_gameOptions[_selectedGameIndex])) {
                    if (_libManager->hasGameLibrary()) {
                        try {
                            // Initialize GameManager with the loaded game from LibraryManager
                            if (_gameManager->initializeFromLibraryManager(*_libManager)) {
                                _state = AppState::GAME;
                            } else {
                                std::cerr << "Failed to initialize game: " << _gameManager->getLastError() << std::endl;
                            }
                        } catch (const std::exception& e) {
                            std::cerr << "Error initializing game: " << e.what() << std::endl;
                        }
                    }
                }
            }
        }
    } else if (_selectedMenuOption == 1) { // Boîte des graphiques
        if (key == IGraphicsLibrary::KEY_UP_CODE) {
            if (_graphicsOptions.size() > 0) {
                _selectedGraphicsIndex = (_selectedGraphicsIndex > 0) ? _selectedGraphicsIndex - 1 : 0;
            }
        } else if (key == IGraphicsLibrary::KEY_DOWN_CODE) {
            if (_graphicsOptions.size() > 0) {
                _selectedGraphicsIndex = (_selectedGraphicsIndex < static_cast<int>(_graphicsOptions.size()) - 1) ? 
                                      _selectedGraphicsIndex + 1 : static_cast<int>(_graphicsOptions.size()) - 1;
            }
        } else if (key == IGraphicsLibrary::KEY_ENTER_CODE) {
            // Charger la bibliothèque sélectionnée uniquement lors de l'appui sur ENTER
            if (_graphicsOptions.size() > 0 && _selectedGraphicsIndex >= 0 && _selectedGraphicsIndex < static_cast<int>(_graphicsOptions.size())) {
                _libManager->loadGraphicsLibrary(_graphicsOptions[_selectedGraphicsIndex]);
            }
        }
    } else if (_selectedMenuOption == 2) { // Boîte du joueur
        if (key == IGraphicsLibrary::KEY_ENTER_CODE) {
            if (_libManager->hasGraphicsLibrary()) {
                try {
                    auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
                    graphicsLib.getPlayerName(_playerName);
                } catch (...) {
                    // Handle exception silently
                }
            }
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
                if (_libManager->hasGameLibrary()) {
                    try {
                        // Initialize GameManager with the loaded game from LibraryManager
                        if (_gameManager->initializeFromLibraryManager(*_libManager)) {
                            _state = AppState::GAME;
                        } else {
                            std::cerr << "Failed to initialize game: " << _gameManager->getLastError() << std::endl;
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Error loading game: " << e.what() << std::endl;
                    }
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
            try {
                // Store current library name to restore if loading fails
                std::string currentLib = _libManager->getCurrentGraphicsLibraryName();
                
                if (_libManager->loadGraphicsLibrary(_graphicsOptions[_selectedSubMenuOption])) {
                    // Successfully loaded
                    _state = AppState::MENU;
                } else {
                    // Loading failed, try to restore previous library
                    std::cerr << "Failed to load graphics library: " << _libManager->getLastError() << std::endl;
                    if (!currentLib.empty()) {
                        _libManager->loadGraphicsLibrary(currentLib);
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error switching graphics library: " << e.what() << std::endl;
            }
        }
        _state = AppState::MENU;
    }
}

void Core::renderMenu() {
    if (!_libManager->hasGraphicsLibrary()) {
        return;
    }
    
    try {
        auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
        
        // Get game names for menu display
        std::vector<std::string> gameNames;
        if (_gameOptions.empty()) {
            gameNames.push_back("No games available");
        } else {
            for (const auto& game : _gameOptions) {
                gameNames.push_back(game);
            }
        }
        
        // Get graphics library names for menu display
        std::vector<std::string> graphicsNames;
        if (_graphicsOptions.empty()) {
            graphicsNames.push_back("No graphics available");
        } else {
            for (const auto& graphics : _graphicsOptions) {
                graphicsNames.push_back(graphics);
            }
        }
        
        // Mettre à jour les indices sélectionnés si nécessaire
        if (_selectedGameIndex >= static_cast<int>(_gameOptions.size())) {
            _selectedGameIndex = _gameOptions.empty() ? 0 : static_cast<int>(_gameOptions.size()) - 1;
        }
        
        if (_selectedGraphicsIndex >= static_cast<int>(_graphicsOptions.size())) {
            _selectedGraphicsIndex = _graphicsOptions.empty() ? 0 : static_cast<int>(_graphicsOptions.size()) - 1;
        }
        
        // Use the standardized menu drawing method
        graphicsLib.drawMenu(
            "ARCADE",
            gameNames,
            graphicsNames,
            _playerName,
            _selectedMenuOption,
            _selectedGameIndex,
            _selectedGraphicsIndex
        );
    } catch (const std::exception& e) {
        std::cerr << "Error rendering menu: " << e.what() << std::endl;
    }
}

void Core::handleGameInput(int key)
{
    if (!_libManager->hasGameLibrary()) {
        _state = AppState::MENU;
        return;
    }

    try {
        auto& game = _libManager->getCurrentGameLibrary();
        
        // First, pass all arrow key inputs to the game
        if (key == IGraphicsLibrary::KEY_LEFT_CODE || 
            key == IGraphicsLibrary::KEY_RIGHT_CODE ||
            key == IGraphicsLibrary::KEY_UP_CODE ||
            key == IGraphicsLibrary::KEY_DOWN_CODE) {
            game.handleInput(key);
            return;
        }
        
        // Then handle other keys
        switch (key) {
            case 'l':
            case 'L':
                _libManager->loadNextGraphicsLibrary();
                break;
            case 'g':
            case 'G':
                if (_libManager->loadNextGameLibrary()) {
                    if (_libManager->hasGameLibrary()) {
                        // Initialize GameManager with the loaded game from LibraryManager
                        _gameManager->initializeFromLibraryManager(*_libManager);
                    }
                }
                break;
            case IGraphicsLibrary::KEY_ESC_CODE:
                _state = AppState::MENU;
                break;
            case 'r':
            case 'R':
                game.restart();
                break;
            case 'q':
            case 'Q':
            case 'm':
            case 'M':
                _state = AppState::MENU;
                break;
            case 'e':
            case 'E':
                _state = AppState::EXIT;
                break;
            default:
                // Pass input directly to the game
                game.handleInput(key);
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error handling input: " << e.what() << std::endl;
    }
}

void Core::renderGame() {
    if (!_libManager->hasGraphicsLibrary()) {
        return;
    }
    
    try {
        auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
        graphicsLib.clear();
        
        // Since the GameManager doesn't have a direct copy of the game,
        // we'll render the game directly from the LibraryManager
        if (_libManager->hasGameLibrary()) {
            auto& game = _libManager->getCurrentGameLibrary();
            game.render(graphicsLib);
            
            // Check if game is over
            if (game.isGameOver()) {
                // Add score
                _scoreManager->addScore(_playerName, game.getName(), game.getScore());
                
                // Return to menu
                _state = AppState::MENU;
            }
        } else {
            // No game loaded, return to menu
            _state = AppState::MENU;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error rendering game: " << e.what() << std::endl;
        _state = AppState::MENU;
    }
}

void Core::renderGameSelection()
{
    if (!_libManager->hasGraphicsLibrary()) {
        return;
    }
    
    try {
        auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
        
        // Calculate dimensions with safety margins
        int width = std::max(40, graphicsLib.getWidth());
        int height = std::max(20, graphicsLib.getHeight());
        
        // Draw title with safe dimensions
        int titleWidth = std::min(40, width - 6);
        int titleHeight = 3;
        int startX = std::max(2, (width - titleWidth) / 2);
        int startY = std::max(2, (height - titleHeight) / 2);
        
        // Ensure we have enough space and margins
        if (startX < 2 || startY < 2 || startY + titleHeight >= height - 2 || startX + titleWidth >= width - 2) {
            return;
        }
        
        graphicsLib.drawBox(startX, startY - 2, titleWidth, titleHeight, Color::CYAN);
        std::string title = "Select Game";
        int titleX = startX + (titleWidth - static_cast<int>(title.length())) / 2;
        graphicsLib.drawText(titleX, startY - 1, title, Color::CYAN);
        
        // Draw game options with safety checks
        int menuStartY = startY + 2;
        if (_gameOptions.empty()) {
            if (menuStartY < height - 2) {
                graphicsLib.drawText(startX, menuStartY, "No games available", Color::RED);
            }
        } else {
            int maxItems = std::min(static_cast<int>(_gameOptions.size()), (height - menuStartY - 2));
            for (int i = 0; i < maxItems; i++) {
                std::string prefix = (i == _selectedSubMenuOption) ? "> " : "  ";
                std::string menuText = prefix + _gameOptions[i];
                if (startX + static_cast<int>(menuText.length()) < width - 2) {
                    graphicsLib.drawText(startX, menuStartY + i, menuText, Color::GREEN);
                }
            }
        }
        
        // Draw controls with bounds checking
        if (height > 4) {
            int controlsY = height - 2;
            std::string controls = "ESC=Back | Enter=Select";
            if (controls.length() < static_cast<size_t>(width - 4)) {
                graphicsLib.drawText(2, controlsY, controls, Color::GREEN);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error rendering game selection: " << e.what() << std::endl;
    }
}

void Core::renderGraphicsSelection()
{
    if (!_libManager->hasGraphicsLibrary()) {
        return;
    }
    
    try {
        auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
        
        // Calculate dimensions with safety margins
        int width = std::max(40, graphicsLib.getWidth());
        int height = std::max(20, graphicsLib.getHeight());
        
        // Draw title with safe dimensions
        int titleWidth = std::min(40, width - 6);
        int titleHeight = 3;
        int startX = std::max(2, (width - titleWidth) / 2);
        int startY = std::max(2, (height - titleHeight) / 2);
        
        // Ensure we have enough space and margins
        if (startX < 2 || startY < 2 || startY + titleHeight >= height - 2 || startX + titleWidth >= width - 2) {
            return;
        }
        
        graphicsLib.drawBox(startX, startY - 2, titleWidth, titleHeight, Color::CYAN);
        std::string title = "Select Graphics Library";
        int titleX = startX + (titleWidth - static_cast<int>(title.length())) / 2;
        graphicsLib.drawText(titleX, startY - 1, title, Color::CYAN);
        
        // Draw graphics options with safety checks
        int menuStartY = startY + 2;
        if (_graphicsOptions.empty()) {
            if (menuStartY < height - 2) {
                graphicsLib.drawText(startX, menuStartY, "No graphics libraries available", Color::RED);
            }
        } else {
            int maxItems = std::min(static_cast<int>(_graphicsOptions.size()), (height - menuStartY - 2));
            for (int i = 0; i < maxItems; i++) {
                std::string prefix = (i == _selectedSubMenuOption) ? "> " : "  ";
                std::string menuText = prefix + _graphicsOptions[i];
                if (startX + static_cast<int>(menuText.length()) < width - 2) {
                    graphicsLib.drawText(startX, menuStartY + i, menuText, Color::GREEN);
                }
            }
        }
        
        // Draw controls with bounds checking
        if (height > 4) {
            int controlsY = height - 2;
            std::string controls = "ESC=Back | Enter=Select";
            if (controls.length() < static_cast<size_t>(width - 4)) {
                graphicsLib.drawText(2, controlsY, controls, Color::GREEN);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error rendering graphics selection: " << e.what() << std::endl;
    }
}

AppState Core::getState() const {
    return _state;
}

std::vector<std::string> Core::getGameLibraries() const
{
    return _libManager->getGameLibraries();
}

std::vector<std::string> Core::getGraphicsLibraries() const
{
    return _libManager->getGraphicsLibraries();
}

} // namespace arcd
