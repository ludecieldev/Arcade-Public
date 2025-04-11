/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** Core
*/

#include "Core.hpp"
#include "utils/Error.hpp"
#include "utils/Event.hpp"
#include "utils/KeyCodes.hpp"
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
    
    // Ne pas charger de jeu par défaut - laisser l'utilisateur choisir
    
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
    
    // Demander le nom du joueur au démarrage s'il n'est pas défini
    if (_playerName == "Player") {
        askPlayerName();
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
        case CoreState::GAME_OVER:
        case CoreState::PAUSE:
        case CoreState::EXIT:
        default:
            // Will be implemented later
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
                case 0: // Select Game
                    updateLibraryLists();
                    _selectedSubMenuOption = 0;
                    _state = CoreState::SELECT_GAME;
                    break;
                case 1: // Select Graphics
                    updateLibraryLists();
                    _selectedSubMenuOption = 0;
                    _state = CoreState::SELECT_GRAPHICS;
                    break;
                case 2: // Leaderboard
                    // TODO: Implement leaderboard view
                    break;
                case 3: // Exit
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
    
    // D'abord essayer de passer l'événement au jeu via le GameManager
    if (_gameManager && _gameManager->hasGame()) {
        // Let the game process the event
        _gameManager->processEvent(event);
        
        // If game is over, save score
        if (_gameManager->isGameOver()) {
            saveScore(_gameManager->getName(), _gameManager->getScore());
        }
    } else if (_libManager && _libManager->hasGameLibrary()) {
        // Sinon, utiliser directement le jeu du LibraryManager
        try {
            auto& gameLib = _libManager->getCurrentGameLibrary();
            gameLib.processEvent(event);
            
            // Si le jeu est terminé, sauvegarder le score
            if (gameLib.isGameOver()) {
                saveScore(gameLib.getName(), gameLib.getScore());
            }
        } catch (const std::exception& e) {
            std::cerr << "Error processing game event: " << e.what() << std::endl;
        }
    }
}

void Core::handleSelectGameEvent(const IEvent& event)
{
    if (event.getType() != EventType::KEY_PRESSED) {
        return;
    }
    
    switch (event.getKeyCode()) {
        case KeyCode::UP:
            _selectedSubMenuOption = (_selectedSubMenuOption > 0) ? 
                _selectedSubMenuOption - 1 : 0;
            break;
        case KeyCode::DOWN:
            _selectedSubMenuOption = (_selectedSubMenuOption < static_cast<int>(_gameOptions.size()) - 1) ? 
                _selectedSubMenuOption + 1 : _gameOptions.size() - 1;
            break;
        case KeyCode::ENTER:
            if (_selectedSubMenuOption >= 0 && _selectedSubMenuOption < static_cast<int>(_gameOptions.size())) {
                // Load the selected game library
                std::string selectedGame = _gameOptions[_selectedSubMenuOption];
                switchGameLibrary(selectedGame);
                
                // Lancer directement le jeu après sélection
                _state = CoreState::GAME;
                if (_gameManager) {
                    _gameManager->resetGame();
                }
            }
            break;
        case KeyCode::ESC:
            _state = CoreState::MENU; // Return to main menu without changing
            break;
    }
}

void Core::handleSelectGraphicsEvent(const IEvent& event)
{
    if (event.getType() != EventType::KEY_PRESSED) {
        return;
    }
    
    switch (event.getKeyCode()) {
        case KeyCode::UP:
            _selectedSubMenuOption = (_selectedSubMenuOption > 0) ? 
                _selectedSubMenuOption - 1 : 0;
            break;
        case KeyCode::DOWN:
            _selectedSubMenuOption = (_selectedSubMenuOption < static_cast<int>(_graphicsOptions.size()) - 1) ? 
                _selectedSubMenuOption + 1 : _graphicsOptions.size() - 1;
            break;
        case KeyCode::ENTER:
            if (_selectedSubMenuOption >= 0 && _selectedSubMenuOption < static_cast<int>(_graphicsOptions.size())) {
                // Save the current library name to avoid loading the same one
                std::string currentLibName = _libManager->getCurrentGraphicsLibraryName();
                
                // Get the selected library
                std::string selectedLib = _graphicsOptions[_selectedSubMenuOption];
                
                // Only switch if it's different from the current one
                if (selectedLib.find(currentLibName) == std::string::npos) {
                    _state = CoreState::MENU; // Set state before switching to avoid render issues
                    switchGraphicsLibrary(selectedLib);
                } else {
                    _state = CoreState::MENU; // Just go back to menu
                }
            }
            break;
        case KeyCode::ESC:
            _state = CoreState::MENU; // Return to main menu without changing
            break;
    }
}

void Core::updateUI()
{
    _uiElements.clear();
    
    // Prepare different UI based on current state
    switch (_state) {
        case CoreState::MENU:
            {
                // Title
                UIElement titleElement = {
                    UIElementType::TEXT,
                    static_cast<int>((_libManager->getCurrentGraphicsLibrary().getWidth() - 6) / 2),
                    2,
                    6, 1,
                    "ARCADE",
                    Color::CYAN,
                    false,
                    {}  // empty properties map
                };
                _uiElements.push_back(titleElement);
                
                // Afficher le nom du joueur
                std::string playerNameText = "Player: " + _playerName;
                UIElement playerNameElement = {
                    UIElementType::TEXT,
                    static_cast<int>((_libManager->getCurrentGraphicsLibrary().getWidth() - playerNameText.length()) / 2),
                    4,
                    static_cast<int>(playerNameText.length()), 1,
                    playerNameText,
                    Color::GREEN,
                    false,
                    {}  // empty properties map
                };
                _uiElements.push_back(playerNameElement);
                
                // Menu options
                for (size_t i = 0; i < _menuOptions.size(); i++) {
                    UIElement menuItemElement = {
                        UIElementType::TEXT,
                        static_cast<int>((_libManager->getCurrentGraphicsLibrary().getWidth() - _menuOptions[i].length()) / 2),
                        7 + static_cast<int>(i) * 2,  // Start lower due to player name
                        static_cast<int>(_menuOptions[i].length()), 1,
                        _menuOptions[i],
                        Color::WHITE,
                        static_cast<int>(i) == _selectedMenuOption,
                        {}  // empty properties map
                    };
                    _uiElements.push_back(menuItemElement);
                }
                
                // Current graphics and game info
                std::string graphicsInfo = "Graphics: " + _libManager->getCurrentGraphicsLibraryName();
                std::string gameInfo = "Game: " + (_gameManager->hasGame() ? _gameManager->getName() : "None");
                
                UIElement graphicsInfoElement = {
                    UIElementType::TEXT,
                    2, _libManager->getCurrentGraphicsLibrary().getHeight() - 2,
                    static_cast<int>(graphicsInfo.length()), 1,
                    graphicsInfo,
                    Color::YELLOW,
                    false,
                    {}  // empty properties map
                };
                _uiElements.push_back(graphicsInfoElement);
                
                UIElement gameInfoElement = {
                    UIElementType::TEXT,
                    2, _libManager->getCurrentGraphicsLibrary().getHeight() - 3,
                    static_cast<int>(gameInfo.length()), 1,
                    gameInfo,
                    Color::YELLOW,
                    false,
                    {}  // empty properties map
                };
                _uiElements.push_back(gameInfoElement);
            }
            break;
            
        case CoreState::GAME:
            {
                // Game state is rendered separately in renderGameState
                // Here we just add UI overlays like score
                if (_gameManager->hasGame()) {
                    std::string scoreText = "Score: " + std::to_string(_gameManager->getScore());
                    UIElement scoreElement = {
                        UIElementType::TEXT,
                        2, 1,
                        static_cast<int>(scoreText.length()), 1,
                        scoreText,
                        Color::YELLOW,
                        false,
                        {}  // empty properties map
                    };
                    _uiElements.push_back(scoreElement);
                    
                    // Help text
                    std::string helpText = "ESC: Menu | 7: Next Game | 9: Next Graphics";
                    UIElement helpElement = {
                        UIElementType::TEXT,
                        static_cast<int>((_libManager->getCurrentGraphicsLibrary().getWidth() - helpText.length()) / 2),
                        _libManager->getCurrentGraphicsLibrary().getHeight() - 1,
                        static_cast<int>(helpText.length()), 1,
                        helpText,
                        Color::WHITE,
                        false,
                        {}  // empty properties map
                    };
                    _uiElements.push_back(helpElement);
                }
            }
            break;
            
        case CoreState::SELECT_GAME:
            {
                // Titre du sous-menu
                UIElement titleElement = {
                    UIElementType::TEXT,
                    static_cast<int>((_libManager->getCurrentGraphicsLibrary().getWidth() - 15) / 2),
                    2,
                    15, 1,
                    "SELECT A GAME",
                    Color::CYAN,
                    false,
                    {}
                };
                _uiElements.push_back(titleElement);
                
                // Liste des jeux disponibles
                for (size_t i = 0; i < _gameOptions.size(); i++) {
                    std::string gameName = _gameOptions[i];
                    // Extraire juste le nom du fichier sans le chemin complet
                    size_t lastSlash = gameName.find_last_of("/\\");
                    if (lastSlash != std::string::npos) {
                        gameName = gameName.substr(lastSlash + 1);
                    }
                    
                    UIElement gameItemElement = {
                        UIElementType::TEXT,
                        static_cast<int>((_libManager->getCurrentGraphicsLibrary().getWidth() - gameName.length()) / 2),
                        6 + static_cast<int>(i) * 2,
                        static_cast<int>(gameName.length()), 1,
                        gameName,
                        Color::WHITE,
                        static_cast<int>(i) == _selectedSubMenuOption,
                        {}
                    };
                    _uiElements.push_back(gameItemElement);
                }
                
                // Instructions
                std::string instructions = "Press ENTER to select, ESC to go back";
                UIElement instructionsElement = {
                    UIElementType::TEXT,
                    static_cast<int>((_libManager->getCurrentGraphicsLibrary().getWidth() - instructions.length()) / 2),
                    _libManager->getCurrentGraphicsLibrary().getHeight() - 3,
                    static_cast<int>(instructions.length()), 1,
                    instructions,
                    Color::YELLOW,
                    false,
                    {}
                };
                _uiElements.push_back(instructionsElement);
            }
            break;
            
        case CoreState::SELECT_GRAPHICS:
            {
                // Titre du sous-menu
                UIElement titleElement = {
                    UIElementType::TEXT,
                    static_cast<int>((_libManager->getCurrentGraphicsLibrary().getWidth() - 19) / 2),
                    2,
                    19, 1,
                    "SELECT A GRAPHICS",
                    Color::CYAN,
                    false,
                    {}
                };
                _uiElements.push_back(titleElement);
                
                // Liste des bibliothèques graphiques disponibles
                for (size_t i = 0; i < _graphicsOptions.size(); i++) {
                    std::string libName = _graphicsOptions[i];
                    // Extraire juste le nom du fichier sans le chemin complet
                    size_t lastSlash = libName.find_last_of("/\\");
                    if (lastSlash != std::string::npos) {
                        libName = libName.substr(lastSlash + 1);
                    }
                    
                    UIElement libItemElement = {
                        UIElementType::TEXT,
                        static_cast<int>((_libManager->getCurrentGraphicsLibrary().getWidth() - libName.length()) / 2),
                        6 + static_cast<int>(i) * 2,
                        static_cast<int>(libName.length()), 1,
                        libName,
                        Color::WHITE,
                        static_cast<int>(i) == _selectedSubMenuOption,
                        {}
                    };
                    _uiElements.push_back(libItemElement);
                }
                
                // Instructions
                std::string instructions = "Press ENTER to select, ESC to go back";
                UIElement instructionsElement = {
                    UIElementType::TEXT,
                    static_cast<int>((_libManager->getCurrentGraphicsLibrary().getWidth() - instructions.length()) / 2),
                    _libManager->getCurrentGraphicsLibrary().getHeight() - 3,
                    static_cast<int>(instructions.length()), 1,
                    instructions,
                    Color::YELLOW,
                    false,
                    {}
                };
                _uiElements.push_back(instructionsElement);
            }
            break;
            
        case CoreState::GAME_OVER:
        case CoreState::PAUSE:
        case CoreState::EXIT:
        default:
            // Ces états seront traités plus tard ou ne nécessitent pas d'UI
            break;
    }
}

void Core::updateGameState()
{
    if (_state == CoreState::GAME) {
        try {
            if (_libManager && _libManager->hasGameLibrary()) {
                auto& gameLib = _libManager->getCurrentGameLibrary();
                _currentGameState = gameLib.getGameState();
            } else if (_gameManager && _gameManager->hasGame()) {
                _currentGameState = _gameManager->getGameState();
            }
        } catch (const std::exception& e) {
            std::cerr << "Error updating game state: " << e.what() << std::endl;
        }
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
    if (_libManager->loadGameLibrary(path)) {
        // Mise à jour du GameManager avec le jeu chargé
        if (_gameManager && _libManager->hasGameLibrary()) {
            try {
                // On n'a pas besoin de faire setGame ici car le jeu est déjà initialisé
                // dans le LibraryManager. On va juste réinitialiser le jeu.
                _gameManager->resetGame();
            } catch (const std::exception& e) {
                std::cerr << "Error updating GameManager: " << e.what() << std::endl;
            }
        }
        return true;
    }
    return false;
}

bool Core::loadGraphicsLibrary(const std::string& path) {
    return _libManager->loadGraphicsLibrary(path);
}

bool Core::switchGameLibrary(const std::string& name) {
    if (_libManager->loadGameLibrary(name)) {
        // Transfert du jeu du LibraryManager au GameManager
        try {
            // Créer une nouvelle instance du jeu via createGameLibrary
            std::string gamePath = name;
            if (_libManager->getGameLibraries().size() > 0) {
                auto it = std::find(_libManager->getGameLibraries().begin(), 
                                  _libManager->getGameLibraries().end(), name);
                if (it != _libManager->getGameLibraries().end()) {
                    gamePath = *it;
                }
            }
            
            if (_gameManager) {
                _gameManager->resetGame();
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in switchGameLibrary: " << e.what() << std::endl;
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
    // Si un nom est fourni, l'utiliser directement
    if (!name.empty()) {
        _playerName = name;
        return;
    }
    
    // Sinon, demander au joueur son nom via la bibliothèque graphique active
    if (_libManager && _libManager->hasGraphicsLibrary()) {
        auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
        graphicsLib.getPlayerName(_playerName);
        
        // Assurer un nom par défaut si aucun n'est entré
        if (_playerName.empty()) {
            _playerName = "Player";
        }
    }
}

std::string Core::getPlayerName() const {
    return _playerName;
}

void Core::saveScore(const std::string& gameName, int score) {
    _scoreManager->addScore(gameName, _playerName, score);
}

std::vector<std::pair<std::string, int>> Core::getHighScores(const std::string& gameName) const
{
    std::vector<Score> gameScores = _scoreManager->getScores(gameName);
    std::vector<std::pair<std::string, int>> result;
    
    // Convertir les Score en pair<string, int>
    for (const auto& score : gameScores) {
        result.emplace_back(score.playerName, score.value);
    }
    
    return result;
}

void Core::askPlayerName() {
    // On utilise la bibliothèque graphique active pour demander le nom du joueur
    if (_libManager && _libManager->hasGraphicsLibrary()) {
        auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
        
        // Préparer l'interface utilisateur pour demander le nom
        std::vector<UIElement> namePromptUI = {
            {
                UIElementType::TEXT,
                static_cast<int>((graphicsLib.getWidth() - 20) / 2),
                (graphicsLib.getHeight() / 2) - 5,
                20, 1,
                "Enter Your Name:",
                Color::CYAN,
                false,
                {}
            }
        };
        
        // Afficher la demande
        graphicsLib.clear();
        graphicsLib.renderUI(namePromptUI);
        graphicsLib.refresh();
        
        // Demander le nom
        std::string tempName;
        graphicsLib.getPlayerName(tempName);
        
        // Mettre à jour le nom du joueur s'il n'est pas vide
        if (!tempName.empty()) {
            _playerName = tempName;
        }
    }
}

} // namespace arcd
