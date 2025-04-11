/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** GameManager
*/

#include "managers/GameManager.hpp"
#include <iostream>

namespace arcd {

GameManager::GameManager() : _isPaused(false)
{
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
}

bool GameManager::isPaused() const
{
    return _isPaused;
}

void GameManager::update(double deltaTime)
{
    if (!_currentGame || _isPaused) {
        return;
    }

    _currentGame->update(deltaTime);
}

void GameManager::processEvent(const IEvent& event)
{
    if (!_currentGame) {
        return;
    }

    // Transmettre l'événement au jeu
    _currentGame->processEvent(event);
}

std::unique_ptr<IGameState> GameManager::getGameState() const
{
    if (!_currentGame) {
        return nullptr;
    }

    return _currentGame->getGameState();
}

bool GameManager::isGameOver() const
{
    if (!_currentGame) {
        return false;
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

std::string GameManager::getDescription() const
{
    if (!_currentGame) {
        return "No game is currently loaded";
    }

    return _currentGame->getDescription();
}

void GameManager::restart()
{
    if (!_currentGame) {
        return;
    }

    _currentGame->restart();
}

} // namespace arcd
