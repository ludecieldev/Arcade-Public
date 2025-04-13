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

int playerOptionSelected = 0;

/**
 * @brief Constructor for Core
 * @param initialGraphicsLib Name of the initial graphics library to load
 */
Core::Core(const std::string& initialGraphicsLib)
    : _state(AppState::MENU), 
      _selectedMenuOption(0), 
      _selectedSubMenuOption(0),
      _selectedGameIndex(0),
      _selectedGraphicsIndex(0),
      _selectedLeaderboardGame(0),
      _isGameOverDisplayed(false)
{
    _libManager = std::make_unique<LibraryManager>("./lib");
    _scoreManager = std::make_unique<ScoreManager>();
    _gameManager = std::make_unique<GameManager>();
    _playerName = "Player";
    
    _libManager->scanLibraries();
    
    if (!_libManager->loadGraphicsLibrary(initialGraphicsLib)) {
        std::string error = "Failed to load initial graphics library: " + _libManager->getLastError();
        throw ArcadeError(error);
    }
    
    auto gameLibs = _libManager->getGameLibraries();
    if (!gameLibs.empty()) {
        if (!_libManager->loadGameLibrary(gameLibs[0])) {
            std::cerr << "Warning: Failed to load initial game library: " << _libManager->getLastError() << std::endl;
        }
    }
    initializeMenu();
}

/**
 * @brief Destructor for Core
 * Cleans up the library manager and game manager
 */
Core::~Core() {
    cleanup();
}

/**
 * @brief Initializes the core application
 * @return True if successful, false otherwise
 */
bool Core::initialize()
{
    if (!_libManager->hasGraphicsLibrary()) {
        return false;
    }

    try {
        _scoreManager->loadScores("./scores.dat");
        
        auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
        return graphicsLib.initialize();
    } catch (const std::exception& e) {
        std::cerr << "Initialization error: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Main loop of the application
 */
void Core::run()
{
    if (!_libManager->hasGraphicsLibrary()) {
        std::cerr << "Error: No graphics library loaded" << std::endl;
        _state = AppState::EXIT;
        return;
    }
    
    if (_libManager->hasGameLibrary()) {
        _gameManager->initializeFromLibraryManager(*_libManager);
    }
    
    while (_state != AppState::EXIT) {
        try {
            if (!_libManager->hasGraphicsLibrary()) {
                std::cerr << "Error: Graphics library was unloaded unexpectedly" << std::endl;
                _state = AppState::EXIT;
                break;
            }
            
            auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
            
            int key = graphicsLib.getKey();
            
            if (key == 'q') {
                _state = AppState::EXIT;
                continue;
            }
            
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
                case AppState::LEADERBOARD:
                    handleLeaderboardInput(key);
                    break;
                default:
                    break;
            }
            
            if (!_libManager->hasGraphicsLibrary()) {
                std::cerr << "Error: Graphics library was unloaded during input handling" << std::endl;
                _state = AppState::EXIT;
                break;
            }
            
            if (_state == AppState::GAME && _libManager->hasGameLibrary()) {
                try {
                    auto& game = _libManager->getCurrentGameLibrary();
                    game.update();
                } catch (const std::exception& e) {
                    std::cerr << "Error updating game: " << e.what() << std::endl;
                }
            }
            
            auto& currentGraphicsLib = _libManager->getCurrentGraphicsLibrary();
            currentGraphicsLib.clear();
            
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
                case AppState::LEADERBOARD:
                    renderLeaderboard();
                    break;
                default:
                    break;
            }
            
            if (!_libManager->hasGraphicsLibrary()) {
                std::cerr << "Error: Graphics library was unloaded during rendering" << std::endl;
                _state = AppState::EXIT;
                break;
            }
            
            currentGraphicsLib.refresh();
        } catch (const std::exception& e) {
            std::cerr << "Error in main loop: " << e.what() << std::endl;
            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 500000000;
            nanosleep(&ts, NULL);
        } catch (...) {
            std::cerr << "Unknown error in main loop" << std::endl;
            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 500000000;
            nanosleep(&ts, NULL);
        }
        
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 50000000;
        nanosleep(&ts, NULL);
    }
}

/**
 * @brief Cleans up the core application
 */
void Core::cleanup() {
    try {
        _scoreManager->saveScores("./scores.dat");
    } catch (const std::exception& e) {
        std::cerr << "Error saving scores: " << e.what() << std::endl;
    }
    
    if (_libManager && _libManager->hasGraphicsLibrary()) {
        try {
            auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
            graphicsLib.cleanup();
            
            _libManager->unloadCurrentGameLibrary();
            _libManager->unloadCurrentGraphicsLibrary();
        } catch (...) {
        }
    }
    
    _state = AppState::EXIT;
    
    _gameManager.reset();
    _libManager.reset();
    _scoreManager.reset();
}

/**
 * @brief Initializes the menu options
 */
void Core::initializeMenu() {
    _menuOptions = {
        "Select Game",
        "Select Graphics Library",
        "Leaderboard",
        "Exit"
    };
    updateLibraryLists();
}

/**
 * @brief Updates the list of available libraries
 */
void Core::updateLibraryLists() {
    _gameOptions = _libManager->getGameLibraries();
    _graphicsOptions = _libManager->getGraphicsLibraries();
    
    _selectedSubMenuOption = 0;
    
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

/**
 * @brief Handles keyboard input in the menu
 * @param key Key pressed by the user
 */
void Core::handleMenuInput(int key)
{
    if (key == IGraphicsLibrary::KEY_ESC_CODE) {
        _state = AppState::EXIT;
        return;
    }
    
    if (key == 'l' || key == 'L') {
        _libManager->loadNextGraphicsLibrary();
        return;
    }
    
    if (key == IGraphicsLibrary::KEY_TAB_CODE) {
        _selectedMenuOption = (_selectedMenuOption + 1) % 3;
        return;
    }
    
    if (_selectedMenuOption == 0) {
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
            if (_gameOptions.size() > 0 && _selectedGameIndex >= 0 && _selectedGameIndex < static_cast<int>(_gameOptions.size())) {
                if (_libManager->loadGameLibrary(_gameOptions[_selectedGameIndex])) {
                    if (_libManager->hasGameLibrary()) {
                        try {
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
    } else if (_selectedMenuOption == 1) {
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
            if (_graphicsOptions.size() > 0 && _selectedGraphicsIndex >= 0 && _selectedGraphicsIndex < static_cast<int>(_graphicsOptions.size())) {
                _libManager->loadGraphicsLibrary(_graphicsOptions[_selectedGraphicsIndex]);
            }
        }
    } else if (_selectedMenuOption == 2) {
        if (key == IGraphicsLibrary::KEY_UP_CODE) {
            playerOptionSelected = 0;
        } else if (key == IGraphicsLibrary::KEY_DOWN_CODE) {
            playerOptionSelected = 1;
        } else if (key == IGraphicsLibrary::KEY_ENTER_CODE) {
            if (playerOptionSelected == 0) {
                if (_libManager->hasGraphicsLibrary()) {
                    try {
                        auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
                        std::string oldName = _playerName;
                        graphicsLib.getPlayerName(_playerName);
                        
                        if (oldName != _playerName && !_playerName.empty()) {
                            std::cout << "Nickname updated to: " << _playerName << std::endl;
                        }
                    } catch (...) {
                    }
                }
            } else {
                _state = AppState::LEADERBOARD;
                _selectedLeaderboardGame = 0;
            }
        }
    }
}

/**
 * @brief Handles keyboard input in the game
 * @param key Key pressed by the user
 */
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

/**
 * @brief Handles keyboard input in the graphics selection menu
 * @param key Key pressed by the user
 */
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
                std::string currentLib = _libManager->getCurrentGraphicsLibraryName();
                
                if (_libManager->loadGraphicsLibrary(_graphicsOptions[_selectedSubMenuOption])) {
                    _state = AppState::MENU;
                } else {
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

/**
 * @brief Handles keyboard input in the leaderboard menu
 * @param key Key pressed by the user
 */
void Core::handleLeaderboardInput(int key)
{
    if (key == IGraphicsLibrary::KEY_ESC_CODE) {
        _state = AppState::MENU;
        return;
    }
    
    if (key == 'l' || key == 'L') {
        _libManager->loadNextGraphicsLibrary();
        return;
    }
    
    if (key == IGraphicsLibrary::KEY_LEFT_CODE) {
        if (!_gameOptions.empty()) {
            _selectedLeaderboardGame = (_selectedLeaderboardGame > 0) ? 
                _selectedLeaderboardGame - 1 : static_cast<int>(_gameOptions.size()) - 1;
        }
    } else if (key == IGraphicsLibrary::KEY_RIGHT_CODE) {
        if (!_gameOptions.empty()) {
            _selectedLeaderboardGame = (_selectedLeaderboardGame + 1) % _gameOptions.size();
        }
    }
}

/**
 * @brief Renders the menu
 */
void Core::renderMenu() {
    if (!_libManager->hasGraphicsLibrary()) {
        return;
    }
    
    try {
        auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
        
        std::vector<std::string> gameNames;
        if (_gameOptions.empty()) {
            gameNames.push_back("No games available");
        } else {
            for (const auto& game : _gameOptions) {
                gameNames.push_back(game);
            }
        }
        
        std::vector<std::string> graphicsNames;
        if (_graphicsOptions.empty()) {
            graphicsNames.push_back("No graphics available");
        } else {
            for (const auto& graphics : _graphicsOptions) {
                graphicsNames.push_back(graphics);
            }
        }
        
        if (_selectedGameIndex >= static_cast<int>(_gameOptions.size())) {
            _selectedGameIndex = _gameOptions.empty() ? 0 : static_cast<int>(_gameOptions.size()) - 1;
        }
        
        if (_selectedGraphicsIndex >= static_cast<int>(_graphicsOptions.size())) {
            _selectedGraphicsIndex = _graphicsOptions.empty() ? 0 : static_cast<int>(_graphicsOptions.size()) - 1;
        }
        
        std::string menuTitle = "ARCADE";
        
        graphicsLib.drawMenu(
            menuTitle,
            gameNames,
            graphicsNames,
            _playerName,
            _selectedMenuOption,
            _selectedGameIndex,
            _selectedGraphicsIndex,
            playerOptionSelected
        );
        
    } catch (const std::exception& e) {
        std::cerr << "Error rendering menu: " << e.what() << std::endl;
    }
}

/**
 * @brief Handles keyboard input in the game
 * @param key Key pressed by the user
 */
void Core::handleGameInput(int key)
{
    if (!_libManager->hasGameLibrary()) {
        _state = AppState::MENU;
        _isGameOverDisplayed = false;
        return;
    }

    try {
        auto& game = _libManager->getCurrentGameLibrary();
        
        if (game.isGameOver()) {
            switch (key) {
                case 'r':
                case 'R':
                    game.restart();
                    _isGameOverDisplayed = false;
                    break;
                case IGraphicsLibrary::KEY_ESC_CODE:
                case 'm':
                case 'M':
                    _state = AppState::MENU;
                    _isGameOverDisplayed = false;
                    break;
                case 'e':
                case 'E':
                    _state = AppState::EXIT;
                    break;
                case 'l':
                case 'L':
                    _libManager->loadNextGraphicsLibrary();
                    break;
                default:
                    break;
            }
            return;
        }
        
        if (key == IGraphicsLibrary::KEY_LEFT_CODE || 
            key == IGraphicsLibrary::KEY_RIGHT_CODE ||
            key == IGraphicsLibrary::KEY_UP_CODE ||
            key == IGraphicsLibrary::KEY_DOWN_CODE) {
            game.handleInput(key);
            return;
        }
        
        switch (key) {
            case 'l':
            case 'L':
                _libManager->loadNextGraphicsLibrary();
                break;
            case 'g':
            case 'G':
                if (_libManager->loadNextGameLibrary()) {
                    if (_libManager->hasGameLibrary()) {
                        _gameManager->initializeFromLibraryManager(*_libManager);
                        _isGameOverDisplayed = false;
                    }
                }
                break;
            case IGraphicsLibrary::KEY_ESC_CODE:
                _state = AppState::MENU;
                _isGameOverDisplayed = false;
                break;
            case 'r':
            case 'R':
                game.restart();
                _isGameOverDisplayed = false;
                break;
            case 'q':
            case 'Q':
            case 'm':
            case 'M':
                _state = AppState::MENU;
                _isGameOverDisplayed = false;
                break;
            case 'e':
            case 'E':
                _state = AppState::EXIT;
                break;
            default:
                game.handleInput(key);
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error handling input: " << e.what() << std::endl;
    }
}

/**
 * @brief Renders the game
 */
void Core::renderGame() {
    if (!_libManager->hasGraphicsLibrary()) {
        return;
    }
    
    try {
        auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
        graphicsLib.clear();
        
        if (_libManager->hasGameLibrary()) {
            auto& game = _libManager->getCurrentGameLibrary();
            
            game.render(graphicsLib);
            
            if (game.isGameOver()) {
                if (!_isGameOverDisplayed) {
                    _scoreManager->addScore(_playerName, game.getName(), game.getScore());
                    _isGameOverDisplayed = true;
                }
            }
        } else {
            _state = AppState::MENU;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error rendering game: " << e.what() << std::endl;
        _state = AppState::MENU;
    }
}

/**
 * @brief Renders the game selection menu
 */
void Core::renderGameSelection()
{
    if (!_libManager->hasGraphicsLibrary()) {
        return;
    }
    
    try {
        auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
        
        int width = std::max(40, graphicsLib.getWidth());
        int height = std::max(20, graphicsLib.getHeight());
        
        int titleWidth = std::min(40, width - 6);
        int titleHeight = 3;
        int startX = std::max(2, (width - titleWidth) / 2);
        int startY = std::max(2, (height - titleHeight) / 2);
        
        if (startX < 2 || startY < 2 || startY + titleHeight >= height - 2 || startX + titleWidth >= width - 2) {
            return;
        }
        
        graphicsLib.drawBox(startX, startY - 2, titleWidth, titleHeight, Color::CYAN);
        std::string title = "Select Game";
        int titleX = startX + (titleWidth - static_cast<int>(title.length())) / 2;
        graphicsLib.drawText(titleX, startY - 1, title, Color::CYAN);
        
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

/**
 * @brief Renders the game selection menu
 */
void Core::renderGraphicsSelection()
{
    if (!_libManager->hasGraphicsLibrary()) {
        return;
    }
    
    try {
        auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
        
        int width = std::max(40, graphicsLib.getWidth());
        int height = std::max(20, graphicsLib.getHeight());
        
        int titleWidth = std::min(40, width - 6);
        int titleHeight = 3;
        int startX = std::max(2, (width - titleWidth) / 2);
        int startY = std::max(2, (height - titleHeight) / 2);
        
        if (startX < 2 || startY < 2 || startY + titleHeight >= height - 2 || startX + titleWidth >= width - 2) {
            return;
        }
        
        graphicsLib.drawBox(startX, startY - 2, titleWidth, titleHeight, Color::CYAN);
        std::string title = "Select Graphics Library";
        int titleX = startX + (titleWidth - static_cast<int>(title.length())) / 2;
        graphicsLib.drawText(titleX, startY - 1, title, Color::CYAN);
        
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

/**
 * @brief Renders the game selection menu
 */
void Core::renderLeaderboard()
{
    if (!_libManager->hasGraphicsLibrary()) {
        return;
    }
    
    try {
        auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
        
        int width = std::max(40, graphicsLib.getWidth());
        int height = std::max(20, graphicsLib.getHeight());
        
        int titleWidth = std::min(40, width - 6);
        int titleHeight = 3;
        int startX = std::max(2, (width - titleWidth) / 2);
        int startY = std::max(2, (height - titleHeight) / 2 - 6);
        
        graphicsLib.drawBox(startX, startY, titleWidth, titleHeight, Color::CYAN);
        std::string title = "LEADERBOARD";
        int titleX = startX + (titleWidth - static_cast<int>(title.length())) / 2;
        graphicsLib.drawText(titleX, startY + 1, title, Color::CYAN);
        
        std::string playerInfo = "Player: " + _playerName;
        int playerInfoX = startX + (titleWidth - static_cast<int>(playerInfo.length())) / 2;
        graphicsLib.drawText(playerInfoX, startY + titleHeight + 1, playerInfo, Color::YELLOW);
        
        std::string gameName = _gameOptions.empty() ? "No games available" : _gameOptions[_selectedLeaderboardGame];
        
        std::string displayGameName = gameName;
        size_t lastSlash = displayGameName.find_last_of('/');
        if (lastSlash != std::string::npos) {
            displayGameName = displayGameName.substr(lastSlash + 1);
        }
        size_t extensionPos = displayGameName.find_last_of('.');
        if (extensionPos != std::string::npos) {
            displayGameName = displayGameName.substr(0, extensionPos);
        }
        
        std::string gameTitle = "Game: " + displayGameName;
        int gameTitleX = startX + (titleWidth - static_cast<int>(gameTitle.length())) / 2;
        graphicsLib.drawText(gameTitleX, startY + 4, gameTitle, Color::GREEN);
        
        int scoreStartY = startY + 6;
        int maxScores = height - scoreStartY - 4;
        
        if (!_gameOptions.empty()) {
            std::string gamePath = gameName;
            std::string realGameName = "";
            
            if (_libManager->hasGameLibrary() && _gameOptions[_selectedLeaderboardGame] == _libManager->getCurrentGameLibraryName()) {
                realGameName = _libManager->getCurrentGameLibrary().getName();
            } else {
                std::string filename = gamePath;
                size_t lastSlash = filename.find_last_of('/');
                if (lastSlash != std::string::npos) {
                    filename = filename.substr(lastSlash + 1);
                }
                
                if (filename.find("arcade_") == 0) {
                    filename = filename.substr(7);
                }
                size_t dotPos = filename.find_last_of('.');
                if (dotPos != std::string::npos) {
                    filename = filename.substr(0, dotPos);
                }
                
                if (!filename.empty()) {
                    filename[0] = std::toupper(filename[0]);
                }
                
                realGameName = filename;
            }
            
            std::vector<Score> scores = _scoreManager->getScores(realGameName);
            
            if (scores.empty()) {
                std::string noScores = "No scores yet for this game";
                int noScoresX = startX + (titleWidth - static_cast<int>(noScores.length())) / 2;
                graphicsLib.drawText(noScoresX, scoreStartY + 2, noScores, Color::YELLOW);
            } else {
                graphicsLib.drawText(startX + 2, scoreStartY, "Player", Color::WHITE);
                graphicsLib.drawText(startX + titleWidth - 10, scoreStartY, "Score", Color::WHITE);
                
                std::string line(titleWidth - 4, '-');
                graphicsLib.drawText(startX + 2, scoreStartY + 1, line, Color::WHITE);
                
                int displayCount = std::min(static_cast<int>(scores.size()), maxScores);
                for (int i = 0; i < displayCount; i++) {
                    std::string playerName = scores[i].playerName;
                    if (playerName.length() > 15) {
                        playerName = playerName.substr(0, 12) + "...";
                    }
                    
                    Color nameColor = (scores[i].playerName == _playerName) ? Color::YELLOW : Color::GREEN;
                    
                    graphicsLib.drawText(startX + 2, scoreStartY + i + 2, playerName, nameColor);
                    
                    std::string scoreText = std::to_string(scores[i].value);
                    graphicsLib.drawText(startX + titleWidth - 5 - static_cast<int>(scoreText.length()), 
                                      scoreStartY + i + 2, scoreText, nameColor);
                }
            }
        } else {
            std::string noGames = "No games available";
            int noGamesX = startX + (titleWidth - static_cast<int>(noGames.length())) / 2;
            graphicsLib.drawText(noGamesX, scoreStartY + 2, noGames, Color::RED);
        }
        
        std::string navigation = "<- Previous Game | Next Game ->";
        int navX = startX + (titleWidth - static_cast<int>(navigation.length())) / 2;
        graphicsLib.drawText(navX, height - 4, navigation, Color::CYAN);
        
        std::string exitHelp = "ESC: Back to Menu";
        int exitX = startX + (titleWidth - static_cast<int>(exitHelp.length())) / 2;
        graphicsLib.drawText(exitX, height - 2, exitHelp, Color::CYAN);
        
    } catch (const std::exception& e) {
        std::cerr << "Error rendering leaderboard: " << e.what() << std::endl;
    }
}

/**
 * @brief Gets the current state of the application
 * @return Current application state
 */
AppState Core::getState() const {
    return _state;
}

/**
 * @brief Gets the player name
 * @return Player name
 */
std::vector<std::string> Core::getGameLibraries() const
{
    return _libManager->getGameLibraries();
}

/**
 * @brief Gets the graphics libraries
 * @return List of graphics libraries
 */
std::vector<std::string> Core::getGraphicsLibraries() const
{
    return _libManager->getGraphicsLibraries();
}

} // namespace arcd
