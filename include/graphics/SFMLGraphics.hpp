/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** SFMLGraphics
*/

#ifndef SFML_GRAPHICS_HPP_
#define SFML_GRAPHICS_HPP_

#include "../interfaces/IGraphicsLibrary.hpp"
#include "../Core.hpp"
#include <string>
#include <vector>
#include <map>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <optional>
#include <functional>

namespace arcd {

// Structure pour stocker les éléments à afficher
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
        int _selectedMenuItem; // Pour stocker l'élément de menu sélectionné
        sf::RenderWindow _window;
        sf::Font _font;
        std::map<int, sf::Color> _colors;
        std::optional<std::reference_wrapper<Core>> _core;

        // Stockage des éléments de dessin
        std::vector<TextDrawElement> _textElements;
        std::vector<RectDrawElement> _rectElements;

        // Helper methods
        void redrawElements();
        void drawTestScreen();
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
        int handleKeyPress(sf::Keyboard::Key key);

    public:
        SFMLGraphics();
        ~SFMLGraphics() override;

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

        // Player name input
        void getPlayerName(std::string& playerName) override;

        // Library information
        std::string getName() const override;

        // Window dimensions
        int getWidth() const override { return _width; }
        int getHeight() const override { return _height; }

        // Set core reference
        void setCore(Core& core) { _core = std::reference_wrapper<Core>(core); }
};

}

extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary();
    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* graphicsLib);
}

#endif // SFML_GRAPHICS_HPP_ 