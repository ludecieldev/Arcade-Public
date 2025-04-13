/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** SDL2Graphics
*/

#ifndef SDL2_GRAPHICS_HPP
#define SDL2_GRAPHICS_HPP

#include "../interfaces/IGraphicsLibrary.hpp"
#include "../Core.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <optional>
#include <functional>

namespace arcd {

/**
 * @brief Graphics library implementation using SDL2
 */
class SDL2Graphics : public IGraphicsLibrary {
    private:
        /**
         * @brief Flag indicating if library has been initialized
         */
        bool _initialized;
        
        /**
         * @brief Width of the display window
         */
        int _width;
        
        /**
         * @brief Height of the display window
         */
        int _height;
        
        /**
         * @brief Last key pressed
         */
        int _lastKey;
        
        /**
         * @brief Counter for animation frames
         */
        int _frameCounter;
        
        /**
         * @brief SDL window handle
         */
        SDL_Window* _window;
        
        /**
         * @brief SDL renderer for drawing
         */
        SDL_Renderer* _renderer;
        
        /**
         * @brief TTF font for text rendering
         */
        TTF_Font* _font;
        
        /**
         * @brief Map of colors for SDL rendering
         */
        std::map<int, SDL_Color> _colors;
        
        /**
         * @brief Optional reference to the core system
         */
        std::optional<std::reference_wrapper<Core>> _core;

        /**
         * @brief Update window size information
         */
        void updateWindowSize();
        
        /**
         * @brief Show splash screen on startup
         */
        void showSplashScreen();
        
        /**
         * @brief Wait for key press with timeout
         * @param timeoutMs timeout in milliseconds
         * @return key code or 0 if timeout
         */
        int waitForKey(int timeoutMs);
        
        /**
         * @brief Wait for any key press
         */
        void waitForAnyKey();

        /**
         * @brief Initialize colors for SDL rendering
         */
        void initColors();

        /**
         * @brief Draw centered text at specified y-coordinate
         * @param y y-coordinate
         * @param text text to draw
         * @param color color to use
         */
        void drawTextCentered(int y, const std::string& text, Color color = Color::DEFAULT);
        
        /**
         * @brief Draw box with a title
         * @param x x-coordinate
         * @param y y-coordinate
         * @param width width of the box
         * @param height height of the box
         * @param title title to display
         * @param color color to use
         */
        void drawBoxWithTitle(int x, int y, int width, int height, const std::string& title, Color color = Color::DEFAULT);
        
        /**
         * @brief Draw filled box with character
         * @param x x-coordinate
         * @param y y-coordinate
         * @param width width of the box
         * @param height height of the box
         * @param fillChar character to fill with
         * @param color color to use
         */
        void drawFilledBox(int x, int y, int width, int height, char fillChar, Color color = Color::DEFAULT);
        
        /**
         * @brief Draw horizontal line
         * @param x x-coordinate
         * @param y y-coordinate
         * @param width width of the line
         * @param color color to use
         */
        void drawHorizontalLine(int x, int y, int width, Color color = Color::DEFAULT);
        
        /**
         * @brief Draw vertical line
         * @param x x-coordinate
         * @param y y-coordinate
         * @param height height of the line
         * @param color color to use
         */
        void drawVerticalLine(int x, int y, int height, Color color = Color::DEFAULT);
        
        /**
         * @brief Draw progress bar
         * @param x x-coordinate
         * @param y y-coordinate
         * @param width width of the bar
         * @param value current value
         * @param maxValue maximum value
         * @param color color to use
         */
        void drawProgressBar(int x, int y, int width, int value, int maxValue, Color color = Color::DEFAULT);
        
        /**
         * @brief Draw bold text
         * @param x x-coordinate
         * @param y y-coordinate
         * @param text text to draw
         * @param color color to use
         */
        void drawBoldText(int x, int y, const std::string& text, Color color);

    public:
        /**
         * @brief Constructor for SDL2Graphics
         */
        SDL2Graphics();
        
        /**
         * @brief Destructor for SDL2Graphics
         */
        ~SDL2Graphics() override;

        /**
         * @brief Initialize the graphics library
         * @return true if initialization succeeded, false otherwise
         */
        bool initialize() override;
        
        /**
         * @brief Clean up resources used by the graphics library
         */
        void cleanup() override;

        /**
         * @brief Clear the screen
         */
        void clear() override;
        
        /**
         * @brief Refresh the display
         */
        void refresh() override;

        /**
         * @brief Draw text at the specified position
         * @param x x-coordinate
         * @param y y-coordinate
         * @param text the text to draw
         * @param color color to use for drawing
         */
        void drawText(int x, int y, const std::string& text, Color color = Color::DEFAULT) override;
        
        /**
         * @brief Draw a box at the specified position
         * @param x x-coordinate
         * @param y y-coordinate
         * @param width width of the box
         * @param height height of the box
         * @param color color to use for drawing
         */
        void drawBox(int x, int y, int width, int height, Color color = Color::DEFAULT) override;
        
        /**
         * @brief Draw a list of items with a selected index
         * @param x x-coordinate
         * @param y y-coordinate
         * @param items vector of strings to display
         * @param selectedIndex index of the selected item
         * @param color color to use for drawing
         */
        void drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color = Color::DEFAULT) override;

        /**
         * @brief Get the last key pressed
         * @return integer code of the key
         */
        int getKey() override;
        
        /**
         * @brief Get the player's name via user input
         * @param playerName reference to string to store the player name
         */
        void getPlayerName(std::string& playerName) override;

        /**
         * @brief Get the name of the graphics library
         * @return name of the graphics library
         */
        std::string getName() const override;

        /**
         * @brief Get the width of the display
         * @return width in pixels
         */
        int getWidth() const override { return _width; }
        
        /**
         * @brief Get the height of the display
         * @return height in pixels
         */
        int getHeight() const override { return _height; }

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
        void drawMenu(
            const std::string& title,
            const std::vector<std::string>& gameOptions,
            const std::vector<std::string>& graphicOptions,
            const std::string& playerName,
            int selectedMenu,
            int selectedGameIndex,
            int selectedGraphicIndex,
            int playerOptionSelected = 0) override;

        /**
         * @brief Set a reference to the core system
         * @param core reference to the Core object
         */
        void setCore(Core& core) { _core = std::reference_wrapper<Core>(core); }
};

}

/**
 * @brief Factory function to create a graphics library instance
 * @return unique pointer to the created graphics library
 */
extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary();
    
    /**
     * @brief Function to destroy a graphics library instance
     * @param graphicsLib pointer to the graphics library to destroy
     */
    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* graphicsLib);
}

#endif // SDL2_GRAPHICS_HPP 