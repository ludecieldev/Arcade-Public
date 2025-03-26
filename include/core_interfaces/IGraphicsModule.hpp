/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** Interface for graphics modules
*/

#ifndef IGRAPHICSMODULE_HPP_
    #define IGRAPHICSMODULE_HPP_

#include <string>
#include "Types.hpp"

namespace arcd {

    /**
     * @brief Interface for graphics modules in the Arcade project.
     *
     * This interface defines the essential methods that all graphics modules must implement.
     * It provides a standardized way for the core to interact with different graphical libraries.
     */
    class IGraphicsModule {
        public:
            virtual ~IGraphicsModule() = default;

            /**
             * @brief Initializes the graphics module.
             */
            virtual void init() = 0;

            /**
             * @brief Stops and cleans up the graphics module.
             */
            virtual void stop() = 0;

            /**
             * @brief Clears the display.
             */
            virtual void clear() = 0;

            /**
             * @brief Updates the display with any pending changes.
             */
            virtual void display() = 0;

            /**
             * @brief Draws text at a specific position.
             *
             * @param txt The text to draw.
             * @param x The x-coordinate of the text position.
             * @param y The y-coordinate of the text position.
             */
            virtual void drawText(const std::string &txt, int x, int y) = 0;

            /**
             * @brief Draws a shape at a specific position.
             *
             * @param type The type of shape to draw.
             * @param x The x-coordinate of the shape position.
             * @param y The y-coordinate of the shape position.
             * @param wdt The width of the shape.
             * @param hgt The height of the shape.
             */
            virtual void drawShape(ShapeType type, int x, int y, int wdt, int hgt) = 0;

            /**
             * @brief Gets the latest event from the graphics module.
             *
             * @return The latest event.
             */
            virtual KeysEvent getEvent() = 0;

            /**
             * @brief Gets the name of the graphics module.
             *
             * @return The name of the graphics module as a string.
             */
            virtual const std::string &getName() const = 0;
    };

}

#endif /* !IGRAPHICSMODULE_HPP_ */
