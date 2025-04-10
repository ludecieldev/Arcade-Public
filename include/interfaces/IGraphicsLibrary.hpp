/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** IGraphicsLibrary
*/

#pragma once

#include <string>
#include <vector>
#include <memory>

namespace arcd {

// Define color constants that can be used by any graphics library
enum class Color {
    DEFAULT,
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE
};

class IGraphicsLibrary {
    public:
        virtual ~IGraphicsLibrary() = default;

        // Key constants
        static constexpr int KEY_ESC_CODE = 27;
        static constexpr int KEY_UP_CODE = 259;
        static constexpr int KEY_DOWN_CODE = 258;
        static constexpr int KEY_LEFT_CODE = 260;
        static constexpr int KEY_RIGHT_CODE = 261;
        static constexpr int KEY_ENTER_CODE = 10;
        static constexpr int KEY_BACKSPACE_CODE = 127;
        static constexpr int KEY_NEXT_LIB_CODE = '9';
        static constexpr int KEY_NEXT_GAME_CODE = '7';

        // Initialization and cleanup
        virtual bool initialize() = 0;
        virtual void cleanup() = 0;

        // Display functions
        virtual void clear() = 0;
        virtual void refresh() = 0;

        // Drawing functions with color support
        virtual void drawText(int x, int y, const std::string& text, Color color = Color::DEFAULT) = 0;
        virtual void drawBox(int x, int y, int width, int height, Color color = Color::DEFAULT) = 0;
        virtual void drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color = Color::DEFAULT) = 0;

        // Input handling - should return standardized key codes
        virtual int getKey() = 0;

        // Player name input
        virtual void getPlayerName(std::string& playerName) = 0;

        // Library information
        virtual std::string getName() const = 0;

        // Window dimensions
        virtual int getWidth() const = 0;
        virtual int getHeight() const = 0;
};

// Standard function for creating a unique_ptr to a graphics library
using create_graphics_t = std::unique_ptr<IGraphicsLibrary> (*)();
// Standard function for handling a graphics library (not needed with unique_ptr)
using destroy_graphics_t = void (*)(IGraphicsLibrary*);
// C-style function signatures for dynamic loading compatibility
extern "C" {
    std::unique_ptr<IGraphicsLibrary> createGraphicsLibrary();
    void destroyGraphicsLibrary(IGraphicsLibrary* lib); // Keep for compatibility
}

}