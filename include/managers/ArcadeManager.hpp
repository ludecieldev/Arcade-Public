/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** ArcadeManager
*/

#ifndef ARCADE_MANAGER_HPP
#define ARCADE_MANAGER_HPP

#include "../interfaces/IGraphicsLibrary.hpp"
#include "../interfaces/IGameLibrary.hpp"
#include "../interfaces/IPlayerManager.hpp"
#include <memory>
#include <vector>
#include <string>

namespace arcd {

    enum class ArcadeState {
        SPLASH_SCREEN,
        MAIN_MENU,
        GAME_SELECTION,
        GRAPHICS_SELECTION,
        PLAYER_INPUT,
        PLAYING_GAME,
        VIEW_SCORES,
        EXIT
    };

    class ArcadeManager {
        private:
            ArcadeState _currentState;
            std::unique_ptr<IPlayerManager> _playerManager;
            std::unique_ptr<IGraphicsLibrary> _currentGraphics;
            std::unique_ptr<IGameLibrary> _currentGame;

            std::vector<std::string> _graphicsLibraryPaths;
            std::vector<std::string> _gameLibraryPaths;
            size_t _currentGraphicsIndex;
            size_t _currentGameIndex;

            int _mainMenuSelection;
            std::vector<std::string> _mainMenuItems;

            // Library loading/unloading
            bool loadGraphicsLibrary(const std::string& path);
            bool loadGameLibrary(const std::string& path);
            void unloadGraphicsLibrary();
            void unloadGameLibrary();

            // Find all available libraries
            void findLibraries();

            // State handlers
            void handleSplashScreen();
            void handleMainMenu();
            void handleGameSelection();
            void handleGraphicsSelection();
            void handlePlayerInput();
            void handlePlayingGame();
            void handleViewScores();

        public:
            ArcadeManager();
            ~ArcadeManager();

            // Initialize the arcade system
            bool initialize(const std::string& initialGraphicsLib);

            // Main loop
            void run();

            // Switch to next graphics/game library
            bool switchToNextGraphicsLibrary();
            bool switchToNextGameLibrary();

            // Restart current game
            void restartGame();
    };

} // namespace arcd

#endif // ARCADE_MANAGER_HPP