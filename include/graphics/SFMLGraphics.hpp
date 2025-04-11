/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** SFMLGraphics
*/

#ifndef SFML_GRAPHICS_HPP_
#define SFML_GRAPHICS_HPP_

#include "../interfaces/IGraphicsLibrary.hpp"
#include "../interfaces/ICore.hpp"
#include "../utils/Event.hpp"
#include "../utils/KeyCodes.hpp"
#include <string>
#include <vector>
#include <map>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <optional>
#include <functional>

namespace arcd {

// Helper structures for rendering
struct TextDrawElement {
    sf::Text text;
    sf::Vector2f position;
};

struct RectDrawElement {
    sf::RectangleShape rect;
};

class SFMLGraphics : public IGraphicsLibrary {
    private:
        bool _initialized;
        int _width;
        int _height;
        int _lastKey;
        int _frameCounter;  // For animations
        sf::RenderWindow _window;
        sf::Font _font;
        std::map<int, sf::Color> _colors;
        std::optional<std::reference_wrapper<ICore>> _core;

        // Drawing elements storage
        std::vector<TextDrawElement> _textElements;
        std::vector<RectDrawElement> _rectElements;

        // Helper methods
        void redrawElements();
        void updateWindowSize();
        void showSplashScreen();
        int waitForKey(int timeoutMs);
        void waitForAnyKey();
        int getKey();
        int handleKeyPress(sf::Keyboard::Key key);

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
        
        // Map SFML key code to our standard key codes
        int mapKeyCode(sf::Keyboard::Key sfmlKey);
        
        // Text and drawing helper methods
        void drawText(int x, int y, const std::string& text, Color color = Color::DEFAULT);
        void drawBox(int x, int y, int width, int height, Color color = Color::DEFAULT);
        void drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color = Color::DEFAULT);
        
        // Entity rendering
        void renderEntity(const Entity& entity);

    public:
        SFMLGraphics();
        ~SFMLGraphics() override;

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
        
        // Player interaction
        void getPlayerName(std::string& playerName) override;

        // Window information
        int getWidth() const override { return _width; }
        int getHeight() const override { return _height; }

        // Library information
        std::string getName() const override;
        std::string getDescription() const override { return "SFML 2.6.2 graphics library"; }

        // Set core reference
        void setCore(ICore& core) { _core = std::reference_wrapper<ICore>(core); }
};

}

extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary();
    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* graphicsLib);
}

#endif // SFML_GRAPHICS_HPP_ 