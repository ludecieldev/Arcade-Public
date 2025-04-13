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

    /**
     * @brief Enumeration of possible states for the arcade system
     */
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

    /**
     * @brief Main manager class for the arcade system
     */
    class ArcadeManager {
        private:
            /**
             * @brief Current state of the arcade system
             */
            ArcadeState _currentState;
            
            /**
             * @brief Player manager instance
             */
            std::unique_ptr<IPlayerManager> _playerManager;
            
            /**
             * @brief Current graphics library instance
             */
            std::unique_ptr<IGraphicsLibrary> _currentGraphics;
            
            /**
             * @brief Current game library instance
             */
            std::unique_ptr<IGameLibrary> _currentGame;

            /**
             * @brief List of available graphics library paths
             */
            std::vector<std::string> _graphicsLibraryPaths;
            
            /**
             * @brief List of available game library paths
             */
            std::vector<std::string> _gameLibraryPaths;
            
            /**
             * @brief Index of the current graphics library
             */
            size_t _currentGraphicsIndex;
            
            /**
             * @brief Index of the current game library
             */
            size_t _currentGameIndex;

            /**
             * @brief Current selection in the main menu
             */
            int _mainMenuSelection;
            
            /**
             * @brief List of main menu items
             */
            std::vector<std::string> _mainMenuItems;

            /**
             * @brief Load a graphics library from the specified path
             * @param path path to the library file
             * @return true if loading succeeded, false otherwise
             */
            bool loadGraphicsLibrary(const std::string& path);
            
            /**
             * @brief Load a game library from the specified path
             * @param path path to the library file
             * @return true if loading succeeded, false otherwise
             */
            bool loadGameLibrary(const std::string& path);
            
            /**
             * @brief Unload the current graphics library
             */
            void unloadGraphicsLibrary();
            
            /**
             * @brief Unload the current game library
             */
            void unloadGameLibrary();

            /**
             * @brief Find all available library files
             */
            void findLibraries();

            /**
             * @brief Handle splash screen state
             */
            void handleSplashScreen();
            
            /**
             * @brief Handle main menu state
             */
            void handleMainMenu();
            
            /**
             * @brief Handle game selection state
             */
            void handleGameSelection();
            
            /**
             * @brief Handle graphics selection state
             */
            void handleGraphicsSelection();
            
            /**
             * @brief Handle player input state
             */
            void handlePlayerInput();
            
            /**
             * @brief Handle playing game state
             */
            void handlePlayingGame();
            
            /**
             * @brief Handle view scores state
             */
            void handleViewScores();

        public:
            /**
             * @brief Constructor for ArcadeManager
             */
            ArcadeManager();
            
            /**
             * @brief Destructor for ArcadeManager
             */
            ~ArcadeManager();

            /**
             * @brief Initialize the arcade system
             * @param initialGraphicsLib path to the initial graphics library
             * @return true if initialization succeeded, false otherwise
             */
            bool initialize(const std::string& initialGraphicsLib);

            /**
             * @brief Run the main arcade loop
             */
            void run();

            /**
             * @brief Switch to the next graphics library
             * @return true if switch succeeded, false otherwise
             */
            bool switchToNextGraphicsLibrary();
            
            /**
             * @brief Switch to the next game library
             * @return true if switch succeeded, false otherwise
             */
            bool switchToNextGameLibrary();

            /**
             * @brief Restart the current game
             */
            void restartGame();
    };

} // namespace arcd

#endif // ARCADE_MANAGER_HPP