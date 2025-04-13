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

    /**
     * @brief Enumeration of possible application states
     */
    enum class AppState {
        MENU,           
        GAME,           
        SELECT_GAME,    
        SELECT_GRAPHICS, 
        LEADERBOARD,    
        EXIT            
    };

    /**
     * @brief Core class managing the overall application flow
     */
    class Core {
        private:
            /**
             * @brief Manager for library loading and handling
             */
            std::unique_ptr<LibraryManager> _libManager;
            
            /**
             * @brief Manager for score tracking
             */
            std::unique_ptr<ScoreManager> _scoreManager;
            
            /**
             * @brief Manager for game operations
             */
            std::unique_ptr<GameManager> _gameManager;
            
            /**
             * @brief Current application state
             */
            AppState _state;
            
            /**
             * @brief Current player name
             */
            std::string _playerName;

            /**
             * @brief Selected option in the main menu
             */
            int _selectedMenuOption;
            
            /**
             * @brief Selected option in the sub-menu
             */
            int _selectedSubMenuOption;
            
            /**
             * @brief Index of selected game
             */
            int _selectedGameIndex;
            
            /**
             * @brief Index of selected graphics library
             */
            int _selectedGraphicsIndex;
            
            /**
             * @brief Index of selected game for leaderboard display
             */
            int _selectedLeaderboardGame;
            
            /**
             * @brief List of main menu options
             */
            std::vector<std::string> _menuOptions;
            
            /**
             * @brief List of available games
             */
            std::vector<std::string> _gameOptions;
            
            /**
             * @brief List of available graphics libraries
             */
            std::vector<std::string> _graphicsOptions;
            
            /**
             * @brief Initialize the menu options
             */
            void initializeMenu();
            
            /**
             * @brief Update the lists of available libraries
             */
            void updateLibraryLists();

            /**
             * @brief Handle input in the menu state
             * @param key key code of pressed key
             */
            void handleMenuInput(int key);
            
            /**
             * @brief Handle input in the game selection state
             * @param key key code of pressed key
             */
            void handleGameSelectionInput(int key);
            
            /**
             * @brief Handle input in the graphics selection state
             * @param key key code of pressed key
             */
            void handleGraphicsSelectionInput(int key);
            
            /**
             * @brief Handle input in the leaderboard state
             * @param key key code of pressed key
             */
            void handleLeaderboardInput(int key);
            
            /**
             * @brief Render the main menu
             */
            void renderMenu();
            
            /**
             * @brief Render the game selection screen
             */
            void renderGameSelection();
            
            /**
             * @brief Render the graphics selection screen
             */
            void renderGraphicsSelection();
            
            /**
             * @brief Render the leaderboard screen
             */
            void renderLeaderboard();

            /**
             * @brief Handle input during gameplay
             * @param key key code of pressed key
             */
            void handleGameInput(int key);
            
            /**
             * @brief Render the current game
             */
            void renderGame();
            
            /**
             * @brief Display an error message
             * @param message error message to display
             */
            void displayError(const std::string& message);

        public:
            /**
             * @brief Constructor for Core
             * @param initialGraphicsLib path to the initial graphics library
             */
            Core(const std::string& initialGraphicsLib);
            
            /**
             * @brief Destructor for Core
             */
            ~Core();

            /**
             * @brief Initialize the core system
             * @return true if initialization succeeded, false otherwise
             */
            bool initialize();
            
            /**
             * @brief Run the main application loop
             */
            void run();
            
            /**
             * @brief Clean up resources
             */
            void cleanup();

            /**
             * @brief Get the current application state
             * @return current application state
             */
            AppState getState() const;
            
            /**
             * @brief Get the player name
             * @return current player name
             */
            const std::string& getPlayerName() const { return _playerName; }
            
            /**
             * @brief Set the player name
             * @param name new player name
             */
            void setPlayerName(const std::string& name) { _playerName = name; }

            /**
             * @brief Get the list of available game libraries
             * @return vector of game library names
             */
            std::vector<std::string> getGameLibraries() const;
            
            /**
             * @brief Get the list of available graphics libraries
             * @return vector of graphics library names
             */
            std::vector<std::string> getGraphicsLibraries() const;
    };

}
#endif // CORE_HPP
