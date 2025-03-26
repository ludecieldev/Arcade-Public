/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** ArcadeCore class header
*/

#ifndef ARCADECORE_HPP_
    #define ARCADECORE_HPP_

#include <string>
#include <memory>
#include "LibraryManager.hpp"
#include "GameManager.hpp"
#include "GraphicsManager.hpp"
#include "Menu.hpp"

namespace arcd {

    class ArcadeCore {
        public:
            ArcadeCore(const std::string &initialGraphicsLib);
            ~ArcadeCore() = default;

            void run();

        private:
            void switchGraphics();
            void switchGame();
            void displayMenu();

            std::unique_ptr<LibraryManager> libraryManager;
            std::unique_ptr<GameManager> gameManager;
            std::unique_ptr<GraphicsManager> graphicsManager;
            std::unique_ptr<Menu> menu;
            bool isRunning;
    };

}

#endif /* !ARCADECORE_HPP_ */
