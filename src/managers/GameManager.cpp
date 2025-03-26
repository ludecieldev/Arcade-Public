/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** GameManager
*/

#include "managers/GameManager.hpp"

namespace arcd {

GameManager::GameManager() : _currentGame(nullptr), _isPaused(false)
{
    _lastUpdateTime = std::chrono::high_resolution_clock::now();
}

GameManager::~GameManager()
{
    // Note: We don't delete _currentGame here because it's managed by LibraryManager
}

bool GameManager::setGame(IGameLibrary* game)
{
    if (!game) {
        return false;
    }
    
    _currentGame = game;
    
    // Initialize the game
    if (!_currentGame->initialize()) {
        _currentGame = nullptr;
        return false;
    }
    
    // Reset the timer
    _lastUpdateTime = std::chrono::high_resolution_clock::now();
    _isPaused = false;
    
    return true;
}

IGameLibrary* GameManager::getCurrentGame() const
{
    return _currentGame;
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
    if (!_currentGame || _isPaused) {
        return;
    }
    
    // Calculate delta time
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - _lastUpdateTime).count();
    _lastUpdateTime = currentTime;
    
    // Update the game
    _currentGame->update(deltaTime);
}

void GameManager::render(IGraphicsLibrary* graphicsLib)
{
    if (!_currentGame || !graphicsLib) {
        return;
    }
    
    // Let the game render itself
    _currentGame->render(graphicsLib);
}

void GameManager::handleInput(int key)
{
    if (!_currentGame) {
        return;
    }
    
    // Handle pause/resume
    if (key == 'p') {
        if (_isPaused) {
            resumeGame();
        } else {
            pauseGame();
        }
        return;
    }
    
    // Handle restart
    if (key == 'r') {
        resetGame();
        return;
    }
    
    // Pass other keys to the game if not paused
    if (!_isPaused) {
        _currentGame->handleInput(key);
    }
}

bool GameManager::isGameOver() const
{
    return _currentGame ? _currentGame->isGameOver() : true;
}

int GameManager::getScore() const
{
    return _currentGame ? _currentGame->getScore() : 0;
}

std::string GameManager::getGameName() const
{
    return _currentGame ? _currentGame->getName() : "No Game";
}

} // namespace arcd
