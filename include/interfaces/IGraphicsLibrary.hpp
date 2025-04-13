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

/**
 * @brief Color enumeration for all graphics libraries
 */
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

/**
 * @brief Interface for graphics libraries
 */
class IGraphicsLibrary {
    public:
        /**
         * @brief Virtual destructor
         */
        virtual ~IGraphicsLibrary() = default;

        /**
         * @brief Key code constants for standardized input handling
         */
        static constexpr int KEY_ESC_CODE = 27;           /**< Escape key */
        static constexpr int KEY_UP_CODE = 259;           /**< Up arrow key */
        static constexpr int KEY_DOWN_CODE = 258;         /**< Down arrow key */
        static constexpr int KEY_LEFT_CODE = 260;         /**< Left arrow key */
        static constexpr int KEY_RIGHT_CODE = 261;        /**< Right arrow key */
        static constexpr int KEY_ENTER_CODE = 10;         /**< Enter key */
        static constexpr int KEY_BACKSPACE_CODE = 127;    /**< Backspace key */
        static constexpr int KEY_NEXT_LIB_CODE = '9';     /**< Switch to next library */
        static constexpr int KEY_NEXT_GAME_CODE = '7';    /**< Switch to next game */
        static constexpr int KEY_SPACE_CODE = ' ';        /**< Space key */
        static constexpr int KEY_TAB_CODE = '\t';         /**< Tab key */
        static constexpr int KEY_RESTART_GAME = 'r';      /**< Restart game key */

        /**
         * @brief Initialize the graphics library
         * @return true if initialization succeeded, false otherwise
         */
        virtual bool initialize() = 0;
        
        /**
         * @brief Clean up resources used by the graphics library
         */
        virtual void cleanup() = 0;

        /**
         * @brief Clear the screen
         */
        virtual void clear() = 0;
        
        /**
         * @brief Refresh the display (swap buffers)
         */
        virtual void refresh() = 0;

        /**
         * @brief Draw text at the specified position
         * @param x x-coordinate
         * @param y y-coordinate
         * @param text the text to draw
         * @param color color to use for drawing
         */
        virtual void drawText(int x, int y, const std::string& text, 
                            Color color = Color::DEFAULT) = 0;

        /**
         * @brief Draw a box at the specified position
         * @param x x-coordinate
         * @param y y-coordinate
         * @param width width of the box
         * @param height height of the box
         * @param color color to use for drawing
         */
        virtual void drawBox(int x, int y, int width, int height, 
                           Color color = Color::DEFAULT) = 0;

        /**
         * @brief Draw a list of items with a selected index
         * @param x x-coordinate
         * @param y y-coordinate
         * @param items vector of strings to display
         * @param selectedIndex index of the selected item
         * @param color color to use for drawing
         */
        virtual void drawList(int x, int y, const std::vector<std::string>& items, 
                            int selectedIndex, Color color = Color::DEFAULT) = 0;

        /**
         * @brief Draw the menu screen
         * @param title title of the menu
         * @param gameOptions list of game options
         * @param graphicOptions list of graphic library options
         * @param playerName name of the player
         * @param selectedMenu selected menu section (0 for games, 1 for graphics)
         * @param selectedGameIndex selected game index
         * @param selectedGraphicIndex selected graphics library index
         */
        virtual void drawMenu(
            const std::string& title,
            const std::vector<std::string>& gameOptions,
            const std::vector<std::string>& graphicOptions,
            const std::string& playerName,
            int selectedMenu,
            int selectedGameIndex,
            int selectedGraphicIndex,
            int playerOptionSelected = 0) = 0;

        /**
         * @brief Get the last key pressed
         * @return integer code of the key
         */
        virtual int getKey() = 0;

        /**
         * @brief Get the player's name via user input
         * @param playerName reference to string to store the player name
         */
        virtual void getPlayerName(std::string& playerName) = 0;

        /**
         * @brief Get the name of the graphics library
         * @return name of the graphics library
         */
        virtual std::string getName() const = 0;

        /**
         * @brief Get the width of the display
         * @return width in pixels/characters
         */
        virtual int getWidth() const = 0;
        
        /**
         * @brief Get the height of the display
         * @return height in pixels/characters
         */
        virtual int getHeight() const = 0;
};

/**
 * @brief Function pointer type for creating a graphics library
 */
using create_graphics_t = std::unique_ptr<IGraphicsLibrary> (*)();

/**
 * @brief Function pointer type for destroying a graphics library
 */
using destroy_graphics_t = void (*)(IGraphicsLibrary*);

/**
 * @brief Factory functions for creating and destroying graphics library instances
 */
extern "C" {
    /**
     * @brief Create a new graphics library instance
     * @return unique pointer to the created graphics library
     */
    std::unique_ptr<IGraphicsLibrary> createGraphicsLibrary();
    
    /**
     * @brief Destroy a graphics library instance (kept for compatibility)
     * @param lib pointer to the graphics library to destroy
     */
    void destroyGraphicsLibrary(IGraphicsLibrary* lib); 
}

} // namespace arcd

#endif // I_GRAPHICS_LIBRARY_HPP