/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** IGraphicsLibrary
*/

#ifndef I_GRAPHICS_LIBRARY_HPP
#define I_GRAPHICS_LIBRARY_HPP

#include "IGameState.hpp"
#include "IEvent.hpp"
#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace arcd {

// Define color constants that can be used by any graphics library
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

// UI Element types for menus, etc.
enum class UIElementType {
    TEXT,
    BUTTON,
    LIST,
    INPUT_FIELD,
    PROGRESS_BAR,
    PANEL,
    CUSTOM
};

// UI Element for Core rendering
struct UIElement {
    UIElementType type;
    int x;
    int y;
    int width;
    int height;
    std::string text;
    Color color;
    bool selected;
    std::map<std::string, std::any> properties;
};

class IGraphicsLibrary {
    public:
        virtual ~IGraphicsLibrary() = default;

        // Initialization and cleanup
        virtual bool initialize() = 0;
        virtual void cleanup() = 0;

        // Display functions
        virtual void clear() = 0;
        virtual void refresh() = 0;

        // Input polling - returns none if no event is available
        virtual std::optional<std::unique_ptr<IEvent>> pollEvent() = 0;

        // Game rendering - renders the game state
        virtual void renderGameState(const IGameState& gameState) = 0;

        // UI rendering - for menus, etc.
        virtual void renderUI(const std::vector<UIElement>& uiElements) = 0;
        
        // Player interaction - for getting player information
        virtual void getPlayerName(std::string& playerName) = 0;

        // Window information
        virtual int getWidth() const = 0;
        virtual int getHeight() const = 0;

        // Library information
        virtual std::string getName() const = 0;
        virtual std::string getDescription() const = 0;
};

// Standard function for creating a unique_ptr to a graphics library
using create_graphics_t = std::unique_ptr<IGraphicsLibrary> (*)();
// Standard function for handling a graphics library (not needed with unique_ptr)
using destroy_graphics_t = void (*)(IGraphicsLibrary*);
// C-style function signatures for dynamic loading compatibility
extern "C" {
    std::unique_ptr<IGraphicsLibrary> createGraphicsLibrary();
    void destroyGraphicsLibrary(IGraphicsLibrary* lib); // Keep for compatibility
}

}

#endif // I_GRAPHICS_LIBRARY_HPP