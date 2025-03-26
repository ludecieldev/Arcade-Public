/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** GameManager class implementation
*/

#include "GameManager.hpp"
#include <iostream>

namespace arcd {

    GameManager::GameManager()
        : libraryManager(std::make_unique<LibraryManager>()),
          currentGameIndex(0)
    {
    }

    void GameManager::loadGame(const std::string &path)
    {
        try {
            currentGame = libraryManager->loadGameLibrary(path);
            currentGamePath = path;

            const std::vector<std::string> &games = libraryManager->getAvailableGameLibraries();

            for (size_t i = 0; i < games.size(); ++i) {
                if (games[i] == path) {
                    currentGameIndex = i;
                    break;
                }
            }
            currentGame->init();
        } catch (const std::exception &e) {
            throw std::runtime_error("Failed to load game library: " + std::string(e.what()));
        }
    }

    bool GameManager::switchToNextGame()
    {
        const std::vector<std::string> &games = libraryManager->getAvailableGameLibraries();

        if (games.empty())
            return false;

        size_t nextIndex = (currentGameIndex + 1) % games.size();
        try {
            loadGame(games[nextIndex]);
            return true;
        } catch (const std::exception &e) {
            std::cerr << "Error switching game: " << e.what() << std::endl;
            return false;
        }
    }

    IGameModule &GameManager::getCurrentGame()
    {
        if (!currentGame)
            throw std::runtime_error("No game loaded");
        return *currentGame;
    }

    const std::string &GameManager::getCurrentGameName() const
    {
        if (!currentGame)
            throw std::runtime_error("No game loaded");
        return currentGame->getName();
    }

}
