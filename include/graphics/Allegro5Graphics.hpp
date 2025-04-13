/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** Allegro5Graphics
*/

#pragma once

#include "../interfaces/IGraphicsLibrary.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace arcd {

/**
 * @brief Graphics library implementation using Allegro5
 */
class Allegro5Graphics : public IGraphicsLibrary {
public:
    /**
     * @brief Constructor for Allegro5Graphics
     */
    Allegro5Graphics();

    /**
     * @brief Destructor for Allegro5Graphics
     */
    ~Allegro5Graphics() override;

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
     * @brief Refresh the display (swap buffers)
     */
    void refresh() override;
    /**
     *
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
    int getWidth() const override;

    /**
     * @brief Get the height of the display
     * @return height in pixels
     */
    int getHeight() const override;

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
     * @brief Draw a test screen to verify graphics functionality
     */
    void drawTestScreen();

private:
    /**
     * @brief Flag indicating if library has been initialized
     */
    bool _initialized;

    /**
     * @brief Width of the display
     */
    int _width;

    /**
     * @brief Height of the display
     */
    int _height;

    /**
     * @brief Current player name
     */
    std::string _playerName;

    /**
     * @brief Last key pressed
     */
    int _lastKey;

    /**
     * @brief Counter for animation frames
     */
    int _frameCounter;

    /**
     * @brief Main Allegro display
     */
    std::unique_ptr<ALLEGRO_DISPLAY, void(*)(ALLEGRO_DISPLAY*)> _display;

    /**
     * @brief Allegro event queue
     */
    std::unique_ptr<ALLEGRO_EVENT_QUEUE, void(*)(ALLEGRO_EVENT_QUEUE*)> _eventQueue;

    /**
     * @brief Font for text rendering
     */
    std::unique_ptr<ALLEGRO_FONT, void(*)(ALLEGRO_FONT*)> _font;

    /**
     * @brief Timer for frame rate control
     */
    std::unique_ptr<ALLEGRO_TIMER, void(*)(ALLEGRO_TIMER*)> _timer;

    /**
     * @brief Flag indicating if window should close
     */
    bool _shouldClose;

    /**
     * @brief Map of colors for the library
     */
    std::map<Color, ALLEGRO_COLOR> _colors;

    /**
     * @brief Initialize color mappings
     */
    void initColors();

    /**
     * @brief Update window dimensions
     */
    void updateWindowSize();

    /**
     * @brief Draw a menu box with title and items
     * @param title title of the box
     * @param items vector of items to display
     * @param x x-coordinate of the box
     * @param y y-coordinate of the box
     * @param width width of the box
     * @param height height of the box
     * @param isSelected whether this box is selected
     */
    void drawMenuBox(const std::string& title, const std::vector<std::string>& items,
                    int x, int y, int width, int height, bool isSelected);
};

} // namespace arcd