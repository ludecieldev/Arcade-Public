/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** NcursesGraphics
*/

#ifndef NCURSES_GRAPHICS_HPP
#define NCURSES_GRAPHICS_HPP

#include "../interfaces/IGraphicsLibrary.hpp"
#include "../Core.hpp"
#include <string>
#include <vector>
#include <map>
#include <termios.h> // For terminal settings
#include <optional>
#include <functional>
#include <ncurses.h>

namespace arcd {

// ANSI color mapping helper
struct ColorMapper {
    static std::string getAnsiCode(Color color);
    static std::string getBrightAnsiCode(Color color);
    static std::string getBold();
    static std::string getReset();
};

class NcursesGraphics : public IGraphicsLibrary {
    private:
        bool _initialized;
        int _width;
        int _height;
        int _lastKey;
        int _frameCounter;  // For animations
        struct termios _oldTermios;  // To restore terminal settings
        std::map<int, int> _colorPairs;
        std::optional<std::reference_wrapper<Core>> _core;

        // Helper methods
        void updateTerminalSize();
        void showSplashScreen();
        int waitForKey(int timeoutMs);
        void waitForAnyKey();

        // Initialize color pairs
        void initColors();

        // Get a color pair index
        int getColorPair(int fg, int bg);

        // Helper method to clear input buffer
        void flushInputBuffer();

        // Helper drawing methods
        void drawTextCentered(int y, const std::string& text, Color color = Color::DEFAULT);
        void drawBoxWithTitle(int x, int y, int width, int height, const std::string& title, Color color = Color::DEFAULT);
        void drawFilledBox(int x, int y, int width, int height, char fillChar, Color color = Color::DEFAULT);
        void drawHorizontalLine(int x, int y, int width, Color color = Color::DEFAULT);
        void drawVerticalLine(int x, int y, int height, Color color = Color::DEFAULT);
        void drawProgressBar(int x, int y, int width, int value, int maxValue, Color color = Color::DEFAULT);
        void drawBoldText(int x, int y, const std::string& text, Color color);

    public:
        NcursesGraphics();
        ~NcursesGraphics() override;

        // Initialization and cleanup
        bool initialize() override;
        void cleanup() override;

        // Display functions
        void clear() override;
        void refresh() override;

        // Drawing functions with color support
        void drawText(int x, int y, const std::string& text, Color color = Color::DEFAULT) override;
        void drawBox(int x, int y, int width, int height, Color color = Color::DEFAULT) override;
        void drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color = Color::DEFAULT) override;

        // Input handling
        int getKey() override;
        MouseEvent getMouse() override;

        // Player name input
        void getPlayerName(std::string& playerName) override;

        // Library information
        std::string getName() const override;

        // Window dimensions
        int getWidth() const override { return _width; }
        int getHeight() const override { return _height; }

        // Set core reference
        void setCore(Core& core) { _core = std::reference_wrapper<Core>(core); }

        // Menu drawing function that all graphics libraries should implement
        void drawMenu(
            const std::string& title,
            const std::vector<std::string>& gameOptions,
            const std::vector<std::string>& graphicOptions,
            const std::string& playerName,
            int selectedMenu,
            int selectedGameIndex,
            int selectedGraphicIndex) override;
};

}

extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary();
    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* graphicsLib);
}

#endif // NCURSES_GRAPHICS_HPP