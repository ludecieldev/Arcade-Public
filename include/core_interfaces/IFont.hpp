/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** Interface for font management
*/

#ifndef IFONT_HPP_
    #define IFONT_HPP_

#include <string>

/**
 * @namespace arcd
 * Namespace used to group all interfaces and classes related to the Arcade project.
 */
namespace arcd {

    /**
     * @brief Interface for managing fonts.
     *
     * This interface defines methods for loading and rendering fonts.
     */
    class IFont {
        public:
            virtual ~IFont() = default;

            /**
             * @brief Loads a font from a file.
             *
             * @param path The file path to the font.
             */
            virtual void loadFont(const std::string &path) = 0;

            /**
             * @brief Sets the size of the font for rendering.
             *
             * @param size The size of the font.
             */
            virtual void setFontSize(int size) = 0;

            /**
             * @brief Draws text using the loaded font at a specific position.
             *
             * @param text The text to render.
             * @param x The horizontal position of the text.
             * @param y The vertical position of the text.
             */
            virtual void drawText(const std::string &text, int x, int y) = 0;
    };

}

#endif /* !IFONT_HPP_ */
