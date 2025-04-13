/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** GameManager
*/

#include "managers/GameManager.hpp"

namespace arcd {

/**
 * @brief Constructor for GameManager
 * @param libDirectory Directory where libraries are located
 */
GameManager::GameManager() : _isPaused(false)
{
    _lastUpdateTime = std::chrono::high_resolution_clock::now();
}

/**
 * @brief Destructor for GameManager
 * Cleans up the current game library if it exists
 */
GameManager::~GameManager()
{
    // Smart pointer will automatically clean up
}

/**
 * @brief Sets the current game library
 * @param game Pointer to the game library to set
 * @return True if successful, false otherwise
 */
bool GameManager::setGame(std::unique_ptr<IGameLibrary> game)
{
    if (!game) {
        _lastError = "Invalid game library";
        return false;
    }

    _currentGame = std::move(game);
    return true;
}

/**
 * @brief Initializes the game manager
 * Resets the state and initializes the current game if it exists
 */
void GameManager::initialize()
{
    // Reset state
    _isPaused = false;
    _lastUpdateTime = std::chrono::high_resolution_clock::now();
    
    // If we have a game, initialize it
    if (_currentGame) {
        try {
            _currentGame->initialize();
        } catch (const std::exception& e) {
            _lastError = "Failed to initialize game: ";
            _lastError += e.what();
        }
    }
}

/**
 * @brief Initializes the game manager from a LibraryManager
 * @param libManager Reference to the LibraryManager
 * @return True if successful, false otherwise
 */
bool GameManager::initializeFromLibraryManager(LibraryManager& libManager)
{
    // Reset state
    _isPaused = false;
    _lastUpdateTime = std::chrono::high_resolution_clock::now();
    
    // Check if the LibraryManager has a game
    if (!libManager.hasGameLibrary()) {
        _lastError = "No game loaded in LibraryManager";
        return false;
    }
    
    try {
        // Get the game from the LibraryManager - we can't directly transfer
        // the unique_ptr, but we'll call the game's methods through the LibraryManager
        
        // Clear our current game first
        _currentGame.reset();
        
        // Update the flag
        _isPaused = false;
        return true;
    } catch (const std::exception& e) {
        _lastError = "Failed to initialize game from LibraryManager: ";
        _lastError += e.what();
        return false;
    }
}

/**
 * @brief Unloads the current game library
 */
IGameLibrary& GameManager::getCurrentGame()
{
    if (!_currentGame) {
        throw std::runtime_error("No game is currently loaded");
    }
    return *_currentGame;
}

/**
 * @brief Checks if a game library is currently loaded
 * @return True if a game library is loaded, false otherwise
 */
bool GameManager::hasGame() const
{
    return _currentGame != nullptr;
}

/**
 * @brief Returns the last error message
 * @return Last error message
 */
void GameManager::resetGame()
{
    if (_currentGame) {
        _currentGame->restart();
        _lastUpdateTime = std::chrono::high_resolution_clock::now();
        _isPaused = false;
    }
}

/**
 * @brief Pauses the game
 */
void GameManager::pauseGame()
{
    _isPaused = true;
}

/**
 * @brief Resumes the game
 */
void GameManager::resumeGame()
{
    _isPaused = false;
    _lastUpdateTime = std::chrono::high_resolution_clock::now();
}

/**
 * @brief Checks if the game is paused
 * @return True if the game is paused, false otherwise
 */
bool GameManager::isPaused() const
{
    return _isPaused;
}

/**
 * @brief Updates the game state
 */
void GameManager::update()
{
    if (!_currentGame) {
        return;
    }

    _currentGame->update();
}

/**
 * @brief Renders the game
 * @param graphics Reference to the graphics library
 */
void GameManager::render(IGraphicsLibrary& graphics)
{
    if (!_currentGame) {
        return;
    }

    _currentGame->render(graphics);
}

/**
 * @brief Handles input for the game
 * @param key Key pressed
 */
void GameManager::handleInput(int key)
{
    if (!_currentGame) {
        return;
    }

    _currentGame->handleInput(key);
}

/**
 * @brief Checks if the game is over
 * @return True if the game is over, false otherwise
 */
bool GameManager::isGameOver() const
{
    if (!_currentGame) {
        return true;
    }

    return _currentGame->isGameOver();
}

/**
 * @brief Returns the score of the current game
 * @return Score of the current game
 */
int GameManager::getScore() const
{
    if (!_currentGame) {
        return 0;
    }

    return _currentGame->getScore();
}

/**
 * @brief Returns the name of the current game
 * @return Name of the current game
 */
std::string GameManager::getName() const
{
    if (!_currentGame) {
        return "No Game";
    }

    return _currentGame->getName();
}

/**
 * @brief Returns the last error message
 * @return Last error message
 */
void GameManager::restart()
{
    if (!_currentGame) {
        return;
    }

    _currentGame->restart();
}

} // namespace arcd
