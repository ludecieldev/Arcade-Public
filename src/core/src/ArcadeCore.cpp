/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** ArcadeCore class implementation
*/

#include "ArcadeCore.hpp"
#include <iostream>

namespace arcd {

    ArcadeCore::ArcadeCore(const std::string &initialGraphicsLib)
        : libraryManager(std::make_unique<LibraryManager>()),
          gameManager(std::make_unique<GameManager>()),
          graphicsManager(std::make_unique<GraphicsManager>()),
          menu(std::make_unique<Menu>()),
          isRunning(true)
    {
        // segfault here
        try {
            graphicsManager->loadLibrary(initialGraphicsLib); // here
        } catch (const std::exception &e) {
            throw std::runtime_error("Failed to load initial graphics library: " + std::string(e.what()));
        }
    }

    void ArcadeCore::run()
    {
        while (isRunning) {
            displayMenu();
            // game loop here
        }
    }

    void ArcadeCore::switchGraphics()
    {
        // switch graphics lib
    }

    void ArcadeCore::switchGame()
    {
        // switch games
    }

    void ArcadeCore::displayMenu()
    {
        menu->display(
            graphicsManager->getCurrentModule(),
            libraryManager->getAvailableGraphicsLibraries(),
            libraryManager->getAvailableGameLibraries(),
            graphicsManager->getCurrentLibraryName(),
            gameManager->getCurrentGameName(),
            "Username"
        );
        // Handle menu interactions
    }

}
