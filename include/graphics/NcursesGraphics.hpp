/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** NcursesGraphics
*/

#ifndef NCURSES_GRAPHICS_HPP
#define NCURSES_GRAPHICS_HPP

#include "../interfaces/IGraphicsLibrary.hpp"
#include <ncurses.h>
#include <string>
#include <vector>
#include <map>
#include <termios.h> // For terminal settings

namespace arcd {

class NcursesGraphics : public IGraphicsLibrary {
private:
    bool _initialized;
    int _width;
    int _height;
    int _lastKey;
    int _frameCounter;  // For animations
    struct termios _oldTermios;  // To restore terminal settings
    std::map<int, int> _colorPairs;
    
    // Initialize color pairs
    void initColors();
    
    // Get a color pair index
    int getColorPair(int fg, int bg);
    
    // Helper method to clear input buffer
    void flushInputBuffer();

public:
    NcursesGraphics();
    ~NcursesGraphics() override;
    
    // Initialization and cleanup
    bool initialize() override;
    void cleanup() override;
    
    // Display functions
    void clear() override;
    void refresh() override;
    
    // Drawing functions
    void drawText(int x, int y, const std::string& text) override;
    void drawBox(int x, int y, int width, int height) override;
    void drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex) override;
    
    // Input handling
    int getKey() override;
    
    // Library information
    std::string getName() const override;
};

}

extern "C" {
    arcd::IGraphicsLibrary* createGraphicsLibrary();
    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* graphicsLib);
}

#endif // NCURSES_GRAPHICS_HPP