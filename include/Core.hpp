/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** Core
*/

#ifndef CORE_HPP
#define CORE_HPP

#include "interfaces/ICore.hpp"
#include "managers/LibraryManager.hpp"
#include "managers/ScoreManager.hpp"
#include "managers/GameManager.hpp"
#include <string>
#include <memory>
#include <chrono>

namespace arcd {

    class Core : public ICore {
        private:
            std::unique_ptr<LibraryManager> _libManager;
            std::unique_ptr<ScoreManager> _scoreManager;
            std::unique_ptr<GameManager> _gameManager;
            CoreState _state;
            std::string _playerName;
            std::chrono::time_point<std::chrono::high_resolution_clock> _lastFrameTime;
            double _deltaTime;
            
            // Current game state cache
            std::unique_ptr<IGameState> _currentGameState;

            // Menu options
            int _selectedMenuOption;
            int _selectedSubMenuOption;
            std::vector<std::string> _menuOptions;
            std::vector<std::string> _gameOptions;
            std::vector<std::string> _graphicsOptions;
            
            // UI Elements
            std::vector<UIElement> _uiElements;
            
            // Methods
            void initializeMenu();
            void updateLibraryLists();
            void updateGameState();
            void updateUI();
            void calculateDeltaTime();
            void processGameEvents();

            // Event handling helpers
            void handleMenuEvent(const IEvent& event);
            void handleGameEvent(const IEvent& event);
            void handleGlobalEvent(const IEvent& event);

        public:
            Core(const std::string& initialGraphicsLib);
            ~Core() override;

            // Core lifecycle - ICore implementation
            bool initialize() override;
            void run() override;
            void cleanup() override;

            // Library management - ICore implementation
            bool loadGameLibrary(const std::string& path) override;
            bool loadGraphicsLibrary(const std::string& path) override;
            bool switchGameLibrary(const std::string& name) override;
            bool switchGraphicsLibrary(const std::string& name) override;
            
            // Get available libraries - ICore implementation
            std::vector<std::string> getAvailableGameLibraries() const override;
            std::vector<std::string> getAvailableGraphicsLibraries() const override;

            // Core state - ICore implementation
            CoreState getState() const override;
            void setState(CoreState state) override;
            
            // Event handling - ICore implementation
            void processEvent(const IEvent& event) override;
            
            // Game state handling - ICore implementation
            const IGameState* getCurrentGameState() const override;
            
            // UI state - ICore implementation
            std::vector<UIElement> getUIElements() const override;
            
            // Player management - ICore implementation
            void setPlayerName(const std::string& name) override;
            std::string getPlayerName() const override;
            
            // Score management - ICore implementation
            void saveScore(const std::string& gameName, int score) override;
            std::vector<std::pair<std::string, int>> getHighScores(const std::string& gameName) const override;
    };

}

#endif // CORE_HPP