/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** Interface for basic display functionalities
*/

#ifndef IDISPLAY_HPP_
    #define IDISPLAY_HPP_

#include <string>

/**
 * @namespace arcd
 * Namespace used to group all interfaces and classes related to the Arcade project.
 */
namespace arcd {

    /**
     * @brief Interface for basic display functionalities.
     *
     * This interface defines essential methods for managing simple graphical rendering,
     * such as clearing the screen, refreshing the display, and drawing text.
     */
    class IDisplay {
        public:
            virtual ~IDisplay() = default;

            /**
             * @brief Clears the screen or window.
             */
            virtual void clear() = 0;

            /**
             * @brief Refreshes the display to show updated content.
             */
            virtual void refresh() = 0;

            /**
             * @brief Draws text at a specific position on the screen.
             *
             * @param text The text to display.
             * @param x The horizontal position of the text.
             * @param y The vertical position of the text.
             */
            virtual void drawText(const std::string &text, int x, int y) = 0;
    };

}

#endif /* !IDISPLAY_HPP_ */
