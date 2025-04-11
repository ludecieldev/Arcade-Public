/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** GameManager
*/

#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include "../interfaces/IGameLibrary.hpp"
#include "../interfaces/IEvent.hpp"
#include <string>
#include <memory>
#include <chrono>
#include <stdexcept>

namespace arcd {

    class GameManager {
        private:
            std::unique_ptr<IGameLibrary> _currentGame;
            bool _isPaused;
            std::string _lastError;

        public:
            GameManager();
            ~GameManager();

            // Game management
            bool setGame(std::unique_ptr<IGameLibrary> game);
            IGameLibrary& getCurrentGame();
            bool hasGame() const;
            void resetGame();

            // Game state
            void pauseGame();
            void resumeGame();
            bool isPaused() const;

            // Game loop
            void update(double deltaTime);
            
            // Event handling
            void processEvent(const IEvent& event);
            
            // Game state
            std::unique_ptr<IGameState> getGameState() const;
            bool isGameOver() const;
            int getScore() const;
            std::string getName() const;
            std::string getDescription() const;
            void restart();
            std::string getLastError() const { return _lastError; }
    };

}

#endif // GAME_MANAGER_HPP
