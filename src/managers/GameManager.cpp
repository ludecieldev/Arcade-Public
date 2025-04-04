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
        _lastError = "Invalid game library";
        return false;
    }

    _currentGame = game;
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
    if (!_currentGame) {
        return;
    }

    _currentGame->update();
}

void GameManager::render(IGraphicsLibrary* graphics)
{
    if (!_currentGame || !graphics) {
        return;
    }

    _currentGame->render(graphics);
}

void GameManager::handleInput(int key)
{
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
