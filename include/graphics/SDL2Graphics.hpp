/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** SDL2Graphics
*/

#ifndef SDL2_GRAPHICS_HPP
#define SDL2_GRAPHICS_HPP

#include "../interfaces/IGraphicsLibrary.hpp"
#include "../interfaces/ICore.hpp"
#include "../utils/Event.hpp"
#include "../utils/KeyCodes.hpp"
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
        std::optional<std::reference_wrapper<ICore>> _core;

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
        
        // Map SDL key code to our standard key codes
        int mapKeyCode(SDL_Keycode sdlKey);
        
        // Text and Box drawing methods
        void drawText(int x, int y, const std::string& text, Color color = Color::DEFAULT);
        void drawBox(int x, int y, int width, int height, Color color = Color::DEFAULT);
        void drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color = Color::DEFAULT);
        
        // Drawing methods for game entities
        void renderEntity(const Entity& entity);

    public:
        SDL2Graphics();
        ~SDL2Graphics() override;

        // Initialization and cleanup
        bool initialize() override;
        void cleanup() override;

        // Display functions
        void clear() override;
        void refresh() override;
        
        // Input polling - returns none if no event is available
        std::optional<std::unique_ptr<IEvent>> pollEvent() override;
        
        // Game rendering - renders the game state
        void renderGameState(const IGameState& gameState) override;
        
        // UI rendering - for menus, etc.
        void renderUI(const std::vector<UIElement>& uiElements) override;

        // Player interaction - pour la saisie du nom du joueur
        void getPlayerName(std::string& playerName) override;

        // Window information
        int getWidth() const override { return _width; }
        int getHeight() const override { return _height; }

        // Library information
        std::string getName() const override;
        std::string getDescription() const override { return "SDL2 graphics library"; }

        // Set core reference
        void setCore(ICore& core) { _core = std::reference_wrapper<ICore>(core); }
};

}

extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary();
    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* graphicsLib);
}

#endif // SDL2_GRAPHICS_HPP 