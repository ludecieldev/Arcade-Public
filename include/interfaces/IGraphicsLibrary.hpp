/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** IGraphicsLibrary
*/

#ifndef I_GRAPHICS_LIBRARY_HPP
#define I_GRAPHICS_LIBRARY_HPP

#include <string>
#include <vector>

namespace arcd {

    class IGraphicsLibrary {
        public:
            virtual ~IGraphicsLibrary() = default;

            // Initialization and cleanup
            virtual bool initialize() = 0;
            virtual void cleanup() = 0;

            // Display functions
            virtual void clear() = 0;
            virtual void refresh() = 0;

            // Drawing functions
            virtual void drawText(int x, int y, const std::string& text) = 0;
            virtual void drawBox(int x, int y, int width, int height) = 0;
            virtual void drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex) = 0;

            // Input handling
            virtual int getKey() = 0;

            // Player name input
            virtual void getPlayerName(std::string& playerName) = 0;

            // Library information
            virtual std::string getName() const = 0;
    };

    // Function signature for the create/destroy functions that must be in the library
    typedef IGraphicsLibrary* (*create_graphics_t)();
    typedef void (*destroy_graphics_t)(IGraphicsLibrary*);

}

#endif // I_GRAPHICS_LIBRARY_HPP