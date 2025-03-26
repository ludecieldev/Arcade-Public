/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** Menu class header
*/

#ifndef MENU_HPP_
    #define MENU_HPP_

#include <string>
#include <vector>
#include "IGraphicsModule.hpp"
#include "Types.hpp"

namespace arcd {

    /**
     * @brief Class for managing the menu interface.
     *
     * This class handles the display and interaction with the main menu,
     * allowing users to select games and graphics libraries.
     */
    class Menu {
        public:
            Menu();
            ~Menu() = default;

            /**
             * @brief Displays the menu using the current graphics module.
             *
             * @param graphics The graphics module to use for display.
             * @param graphicsLibs List of available graphics libraries.
             * @param gameLibs List of available game libraries.
             * @param currentGraphics Name of the current graphics library.
             * @param currentGame Name of the current game (if any).
             * @param playerName Current player name.
             */
            void display(IGraphicsModule &graphics,
                        const std::vector<std::string> &graphicsLibs,
                        const std::vector<std::string> &gameLibs,
                        const std::string &currentGraphics,
                        const std::string &currentGame,
                        const std::string &playerName);

            /**
             * @brief Handles user input in the menu.
             *
             * @param event The event to handle.
             * @return true if a game should be started, false otherwise.
             */
            bool handleEvent(KeysEvent event);

            /**
             * @brief Gets the selected game.
             *
             * @return The index of the selected game.
             */
            size_t getSelectedGame() const;

            /**
             * @brief Gets the selected graphics library.
             *
             * @return The index of the selected graphics library.
             */
            size_t getSelectedGraphics() const;

            /**
             * @brief Gets the player name.
             *
             * @return The player name.
             */
            const std::string &getPlayerName() const;

        private:
            /**
             * @brief Draws a box on the screen.
             *
             * @param graphics The graphics module to use.
             * @param x The x-coordinate of the box.
             * @param y The y-coordinate of the box.
             * @param width The width of the box.
             * @param height The height of the box.
             * @param title The title of the box.
             */
            void drawBox(IGraphicsModule &graphics, int x, int y, int width, int height, const std::string &title);

            size_t selectedGame;
            size_t selectedGraphics;
            std::string playerName;
            bool isEditingName;
    };

}

#endif /* !MENU_HPP_ */
