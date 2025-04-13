/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** Core
*/

#ifndef CORE_HPP
#define CORE_HPP

#include "managers/LibraryManager.hpp"
#include "managers/ScoreManager.hpp"
#include "managers/GameManager.hpp"
#include <string>
#include <memory>

namespace arcd {

    enum class AppState {
        MENU,
        GAME,
        SELECT_GAME,
        SELECT_GRAPHICS,
        LEADERBOARD,
        EXIT
    };

    class Core {
        private:
            std::unique_ptr<LibraryManager> _libManager;
            std::unique_ptr<ScoreManager> _scoreManager;
            std::unique_ptr<GameManager> _gameManager;
            AppState _state;
            std::string _playerName;

            // Menu options
            int _selectedMenuOption;
            int _selectedSubMenuOption;
            int _selectedGameIndex;
            int _selectedGraphicsIndex;
            int _selectedLeaderboardGame; // Index of currently selected game for leaderboard
            std::vector<std::string> _menuOptions;
            std::vector<std::string> _gameOptions;
            std::vector<std::string> _graphicsOptions;
            void initializeMenu();
            void updateLibraryLists();

            void handleMenuInput(int key);
            void handleGameSelectionInput(int key);
            void handleGraphicsSelectionInput(int key);
            void handleLeaderboardInput(int key);
            void renderMenu();
            void renderGameSelection();
            void renderGraphicsSelection();
            void renderLeaderboard();

            void handleGameInput(int key);
            void renderGame();
            void displayError(const std::string& message);

        public:
            Core(const std::string& initialGraphicsLib);
            ~Core();

            bool initialize();
            void run();
            void cleanup();

            // Getters
            AppState getState() const;
            const std::string& getPlayerName() const { return _playerName; }
            void setPlayerName(const std::string& name) { _playerName = name; }

            // Modified methods to return copies instead of references
            std::vector<std::string> getGameLibraries() const;
            std::vector<std::string> getGraphicsLibraries() const;
    };

}
#endif // CORE_HPP
