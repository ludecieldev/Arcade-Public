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

class SDL2Graphics : public IGraphicsLibrary {
    private:
        bool _initialized;
        int _width;
        int _height;
        int _lastKey;
        int _frameCounter;  // For animations
        SDL_Window* _window;
        SDL_Renderer* _renderer;
        TTF_Font* _font;
        std::map<int, SDL_Color> _colors;
        std::optional<std::reference_wrapper<Core>> _core;

        // Helper methods
        void updateWindowSize();
        void showSplashScreen();
        int waitForKey(int timeoutMs);
        void waitForAnyKey();

        // Initialize colors
        void initColors();

        // Helper drawing methods
        void drawTextCentered(int y, const std::string& text, Color color = Color::DEFAULT);
        void drawBoxWithTitle(int x, int y, int width, int height, const std::string& title, Color color = Color::DEFAULT);
        void drawFilledBox(int x, int y, int width, int height, char fillChar, Color color = Color::DEFAULT);
        void drawHorizontalLine(int x, int y, int width, Color color = Color::DEFAULT);
        void drawVerticalLine(int x, int y, int height, Color color = Color::DEFAULT);
        void drawProgressBar(int x, int y, int width, int value, int maxValue, Color color = Color::DEFAULT);
        void drawBoldText(int x, int y, const std::string& text, Color color);

    public:
        SDL2Graphics();
        ~SDL2Graphics() override;

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
        void getPlayerName(std::string& playerName) override;

        // Library information
        std::string getName() const override;

        // Window dimensions
        int getWidth() const override { return _width; }
        int getHeight() const override { return _height; }

        // Menu drawing function that all graphics libraries should implement
        void drawMenu(
            const std::string& title,
            const std::vector<std::string>& gameOptions,
            const std::vector<std::string>& graphicOptions,
            const std::string& playerName,
            int selectedMenu,
            int selectedGameIndex,
            int selectedGraphicIndex) override;

        // Set core reference
        void setCore(Core& core) { _core = std::reference_wrapper<Core>(core); }
};

}

extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary();
    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* graphicsLib);
}

#endif // SDL2_GRAPHICS_HPP 