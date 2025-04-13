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
#include <memory>

namespace arcd {

//=============================================================================
// Color enumeration for all graphics libraries
//=============================================================================
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

        //=====================================================================
        // Key code constants
        //=====================================================================
        static constexpr int KEY_ESC_CODE = 27;
        static constexpr int KEY_UP_CODE = 259;
        static constexpr int KEY_DOWN_CODE = 258;
        static constexpr int KEY_LEFT_CODE = 260;
        static constexpr int KEY_RIGHT_CODE = 261;
        static constexpr int KEY_ENTER_CODE = 10;
        static constexpr int KEY_BACKSPACE_CODE = 127;
        static constexpr int KEY_NEXT_LIB_CODE = '9';
        static constexpr int KEY_NEXT_GAME_CODE = '7';
        static constexpr int KEY_SPACE_CODE = ' ';
        static constexpr int KEY_TAB_CODE = '\t';
        static constexpr int KEY_RESTART_GAME = 'r';

        //=====================================================================
        // Mouse constants
        //=====================================================================
        static constexpr int MOUSE_LEFT_BUTTON = 1;
        static constexpr int MOUSE_RIGHT_BUTTON = 2;
        static constexpr int MOUSE_MIDDLE_BUTTON = 3;

        struct MouseEvent {
            bool hasEvent = false;
            int x = 0;
            int y = 0;
            bool leftButton = false;
            bool rightButton = false;
        };

        //=====================================================================
        // Core functions
        //=====================================================================

        // Initialization and cleanup
        virtual bool initialize() = 0;
        virtual void cleanup() = 0;

        // Display functions
        virtual void clear() = 0;
        virtual void refresh() = 0;


        //=====================================================================
        // Drawing functions
        //=====================================================================
        virtual void drawText(int x, int y, const std::string& text, 
                            Color color = Color::DEFAULT) = 0;

        virtual void drawBox(int x, int y, int width, int height, 
                           Color color = Color::DEFAULT) = 0;

        virtual void drawList(int x, int y, const std::vector<std::string>& items, 
                            int selectedIndex, Color color = Color::DEFAULT) = 0;

        // Menu drawing function that all graphics libraries should implement
        virtual void drawMenu(
            const std::string& title,
            const std::vector<std::string>& gameOptions,
            const std::vector<std::string>& graphicOptions,
            const std::string& playerName,
            int selectedMenu,
            int selectedGameIndex,
            int selectedGraphicIndex) = 0;


        //=====================================================================
        // Input and state functions
        //=====================================================================

        // Input handling - returns standardized key codes
        virtual int getKey() = 0;

        // Get a mouse event (left button click, right button click, position)
        virtual MouseEvent getMouse() { 
            // Default implementation returns no event to avoid crashes
            return MouseEvent{false, 0, 0, false, false}; 
        }

        // Player name management
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

} // namespace arcd

#endif // I_GRAPHICS_LIBRARY_HPP