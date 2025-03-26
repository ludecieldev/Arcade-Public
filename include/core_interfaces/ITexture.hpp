/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** Interface for texture management
*/

#ifndef ITEXTURE_HPP_
    #define ITEXTURE_HPP_

#include <string>

/**
 * @namespace arcd
 * Namespace used to group all interfaces and classes related to the Arcade project.
 */
namespace arcd {

    /**
     * @brief Interface for managing textures.
     *
     * This interface defines methods for loading, unloading, and rendering textures.
     */
    class ITexture {
        public:
            virtual ~ITexture() = default;

            /**
             * @brief Loads a texture from a file.
             *
             * @param path The file path to the texture.
             */
            virtual void loadTexture(const std::string &path) = 0;

            /**
             * @brief Unloads a previously loaded texture.
             *
             * @param id The ID of the texture to unload.
             */
            virtual void unloadTexture(int id) = 0;

            /**
             * @brief Draws a texture on the screen at a specific position.
             *
             * @param id The ID of the texture to draw.
             * @param x The horizontal position of the texture.
             * @param y The vertical position of the texture.
             */
            virtual void drawTexture(int id, int x, int y) = 0;
    };

}

#endif /* !ITEXTURE_HPP_ */
