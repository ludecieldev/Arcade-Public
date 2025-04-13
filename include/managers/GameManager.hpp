/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** GameManager
*/

#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include "../interfaces/IGameLibrary.hpp"
#include "../interfaces/IGraphicsLibrary.hpp"
#include "../managers/LibraryManager.hpp"
#include <string>
#include <memory>
#include <chrono>
#include <stdexcept>

namespace arcd {

    /**
     * @brief Manager class for game operations and lifecycle
     */
    class GameManager {
        private:
            /**
             * @brief Currently loaded game library
             */
            std::unique_ptr<IGameLibrary> _currentGame;
            
            /**
             * @brief Timestamp of the last game update
             */
            std::chrono::time_point<std::chrono::high_resolution_clock> _lastUpdateTime;
            
            /**
             * @brief Flag indicating if the game is paused
             */
            bool _isPaused;
            
            /**
             * @brief Last error message
             */
            std::string _lastError;

        public:
            /**
             * @brief Constructor for GameManager
             */
            GameManager();
            
            /**
             * @brief Destructor for GameManager
             */
            ~GameManager();

            /**
             * @brief Set the current game
             * @param game unique pointer to game library
             * @return true if game was set successfully, false otherwise
             */
            bool setGame(std::unique_ptr<IGameLibrary> game);
            
            /**
             * @brief Basic initialization of the game
             */
            void initialize();
            
            /**
             * @brief Initialize the game from a LibraryManager
             * @param libManager reference to the library manager
             * @return true if initialization succeeded, false otherwise
             */
            bool initializeFromLibraryManager(LibraryManager& libManager);
            
            /**
             * @brief Get the current game
             * @return reference to the current game library
             */
            IGameLibrary& getCurrentGame();
            
            /**
             * @brief Check if a game is loaded
             * @return true if a game is loaded, false otherwise
             */
            bool hasGame() const;
            
            /**
             * @brief Reset the current game
             */
            void resetGame();

            /**
             * @brief Pause the current game
             */
            void pauseGame();
            
            /**
             * @brief Resume the current game
             */
            void resumeGame();
            
            /**
             * @brief Check if the game is paused
             * @return true if the game is paused, false otherwise
             */
            bool isPaused() const;

            /**
             * @brief Update the game state
             */
            void update();
            
            /**
             * @brief Render the game using the provided graphics library
             * @param graphicsLib reference to the graphics library
             */
            void render(IGraphicsLibrary& graphicsLib);

            /**
             * @brief Handle user input
             * @param key key code of the pressed key
             */
            void handleInput(int key);

            /**
             * @brief Check if the game is over
             * @return true if the game is over, false otherwise
             */
            bool isGameOver() const;
            
            /**
             * @brief Get the current score
             * @return current score
             */
            int getScore() const;
            
            /**
             * @brief Get the name of the current game
             * @return name of the game
             */
            std::string getName() const;
            
            /**
             * @brief Restart the current game
             */
            void restart();
            
            /**
             * @brief Get the last error message
             * @return last error message
             */
            std::string getLastError() const { return _lastError; }
    };

}

#endif // GAME_MANAGER_HPP
