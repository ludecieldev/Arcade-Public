/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** GameManager
*/

#pragma once

#include "../interfaces/IGameLibrary.hpp"
#include "../interfaces/IGraphicsLibrary.hpp"
#include <string>
#include <memory>
#include <chrono>

namespace arcd {

    class GameManager {
        private:
            IGameLibrary* _currentGame;
            std::chrono::time_point<std::chrono::high_resolution_clock> _lastUpdateTime;
            bool _isPaused;
            std::string _lastError;

        public:
            GameManager();
            ~GameManager();

            // Game management
            bool setGame(IGameLibrary* game);
            IGameLibrary* getCurrentGame() const;
            void resetGame();

            // Game state
            void pauseGame();
            void resumeGame();
            bool isPaused() const;

            // Game loop
            void update();
            void render(IGraphicsLibrary* graphicsLib);

            // Input handling
            void handleInput(int key);

            // Game information
            bool isGameOver() const;
            int getScore() const;
            std::string getName() const;
            void restart();
            std::string getLastError() const { return _lastError; }
    };

}
