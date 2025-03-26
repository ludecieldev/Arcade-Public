/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** GameManager class header
*/

#ifndef GAMEMANAGER_HPP_
    #define GAMEMANAGER_HPP_

#include <memory>
#include <string>
#include "LibraryManager.hpp"
#include "IGameModule.hpp"

namespace arcd {

    /**
     * @brief Class for managing game libraries.
     *
     * This class handles the loading and interaction with game libraries.
     */
    class GameManager {
        public:
            GameManager();
            ~GameManager() = default;

            /**
             * @brief Loads a game library.
             *
             * @param path The path to the library file.
             * @throw std::runtime_error If the library cannot be loaded.
             */
            void loadGame(const std::string &path);

            /**
             * @brief Switches to the next available game.
             *
             * @return true if the switch was successful, false otherwise.
             */
            bool switchToNextGame();

            /**
             * @brief Gets the current game module.
             *
             * @return A reference to the current game module.
             */
            IGameModule &getCurrentGame();

            /**
             * @brief Gets the name of the current game.
             *
             * @return The name of the current game.
             */
            const std::string &getCurrentGameName() const;

        private:
            std::unique_ptr<LibraryManager> libraryManager;
            std::shared_ptr<IGameModule> currentGame;
            std::string currentGamePath;
            size_t currentGameIndex;
    };

}

#endif /* !GAMEMANAGER_HPP_ */
