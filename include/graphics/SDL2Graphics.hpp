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
 * Graphics library implementation using SDL2
 * Encapsulates SDL2 functionality with modern C++ practices
 */
class SDL2Graphics : public IGraphicsLibrary {
    private:
        // Flag indicating initialization state
        bool _initialized;
        
        // Display window dimensions
        int _width;
        int _height;
        
        // Input and animation state
        int _lastKey;
        int _frameCounter;
        
        // Smart pointers for SDL resources with custom deleters
        struct SDLDeleter {
            void operator()(SDL_Window* window) { if (window) SDL_DestroyWindow(window); }
            void operator()(SDL_Renderer* renderer) { if (renderer) SDL_DestroyRenderer(renderer); }
            void operator()(TTF_Font* font) { if (font) TTF_CloseFont(font); }
            void operator()(SDL_Surface* surface) { if (surface) SDL_FreeSurface(surface); }
            void operator()(SDL_Texture* texture) { if (texture) SDL_DestroyTexture(texture); }
        };
        
        // SDL resources managed with smart pointers
        std::unique_ptr<SDL_Window, SDLDeleter> _window;
        std::unique_ptr<SDL_Renderer, SDLDeleter> _renderer;
        std::unique_ptr<TTF_Font, SDLDeleter> _font;
        
        // Color mapping for SDL rendering
        std::map<int, SDL_Color> _colors;
        
        // Optional reference to the core system
        std::optional<std::reference_wrapper<Core>> _core;

        // Helper functions for window and display management
        void updateWindowSize();
        void showSplashScreen();
        int waitForKey(int timeoutMs);
        void waitForAnyKey();
        void initColors();

        // Wrapper functions for SDL drawing operations
        void drawTextCentered(int y, const std::string& text, Color color = Color::DEFAULT);
        void drawBoxWithTitle(int x, int y, int width, int height, const std::string& title, Color color = Color::DEFAULT);
        void drawFilledBox(int x, int y, int width, int height, char fillChar, Color color = Color::DEFAULT);
        void drawHorizontalLine(int x, int y, int width, Color color = Color::DEFAULT);
        void drawVerticalLine(int x, int y, int height, Color color = Color::DEFAULT);
        void drawProgressBar(int x, int y, int width, int value, int maxValue, Color color = Color::DEFAULT);
        void drawBoldText(int x, int y, const std::string& text, Color color);

        // Utility functions for SDL operations
        std::unique_ptr<SDL_Surface, SDLDeleter> createTextSurface(const std::string& text, Color color);
        std::unique_ptr<SDL_Texture, SDLDeleter> createTextureFromSurface(SDL_Surface* surface);

    public:
        // Constructor and destructor
        SDL2Graphics();
        ~SDL2Graphics() override;

        // IGraphicsLibrary interface implementation
        bool initialize() override;
        void cleanup() override;
        void clear() override;
        void refresh() override;
        void drawText(int x, int y, const std::string& text, Color color = Color::DEFAULT) override;
        void drawBox(int x, int y, int width, int height, Color color = Color::DEFAULT) override;
        void drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color = Color::DEFAULT) override;
        int getKey() override;
        void getPlayerName(std::string& playerName) override;
        std::string getName() const override;
        int getWidth() const override { return _width; }
        int getHeight() const override { return _height; }

        // Menu drawing functionality
        void drawMenu(
            const std::string& title,
            const std::vector<std::string>& gameOptions,
            const std::vector<std::string>& graphicOptions,
            const std::string& playerName,
            int selectedMenu,
            int selectedGameIndex,
            int selectedGraphicIndex,
            int playerOptionSelected = 0) override;

        // Core system integration
        void setCore(Core& core) { _core = std::reference_wrapper<Core>(core); }
};

}

// External C interface for dynamic loading
extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary();
    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* graphicsLib);
}

#endif // SDL2_GRAPHICS_HPP 