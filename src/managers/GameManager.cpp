/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** GameManager
*/

#include "managers/GameManager.hpp"

namespace arcd {

GameManager::GameManager() : _isPaused(false)
{
    _lastUpdateTime = std::chrono::high_resolution_clock::now();
}

GameManager::~GameManager()
{
    // Smart pointer will automatically clean up
}

bool GameManager::setGame(std::unique_ptr<IGameLibrary> game)
{
    if (!game) {
        _lastError = "Invalid game library";
        return false;
    }

    _currentGame = std::move(game);
    return true;
}

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

IGameLibrary& GameManager::getCurrentGame()
{
    if (!_currentGame) {
        throw std::runtime_error("No game is currently loaded");
    }
    return *_currentGame;
}

bool GameManager::hasGame() const
{
    return _currentGame != nullptr;
}

void GameManager::resetGame()
{
    if (_currentGame) {
        _currentGame->restart();
        _lastUpdateTime = std::chrono::high_resolution_clock::now();
        _isPaused = false;
    }
}

void GameManager::pauseGame()
{
    _isPaused = true;
}

void GameManager::resumeGame()
{
    _isPaused = false;
    _lastUpdateTime = std::chrono::high_resolution_clock::now();
}

bool GameManager::isPaused() const
{
    return _isPaused;
}

void GameManager::update()
{
    if (!_currentGame) {
        return;
    }

    _currentGame->update();
}

void GameManager::render(IGraphicsLibrary& graphics)
{
    // When using initializeFromLibraryManager, we don't have our own copy of the game,
    // so this function needs to handle that case differently.
    // 
    // For now, we just do nothing if no local game is available.
    // Later, we should update the Core to pass the LibraryManager instead so we can
    // access the game through it.
    if (!_currentGame) {
        return;
    }

    _currentGame->render(graphics);
}

void GameManager::handleInput(int key)
{
    // When using initializeFromLibraryManager, we don't have our own copy of the game,
    // so we do nothing here. The input is handled directly by Core.cpp which passes
    // the input to the game in LibraryManager.
    if (!_currentGame) {
        return;
    }

    _currentGame->handleInput(key);
}

bool GameManager::isGameOver() const
{
    if (!_currentGame) {
        return true;
    }

    return _currentGame->isGameOver();
}

int GameManager::getScore() const
{
    if (!_currentGame) {
        return 0;
    }

    return _currentGame->getScore();
}

std::string GameManager::getName() const
{
    if (!_currentGame) {
        return "No Game";
    }

    return _currentGame->getName();
}

void GameManager::restart()
{
    if (!_currentGame) {
        return;
    }

    _currentGame->restart();
}

} // namespace arcd
