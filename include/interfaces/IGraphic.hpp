#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace arcd {

struct Input {
    enum class Type {
        KEYBOARD,
        MOUSE,
        NONE
    };

    Type type;
    int key;
    int x;
    int y;
    bool pressed;
};

struct GameState {
    struct Entity {
        enum class Type {
            RECTANGLE,
            CIRCLE,
            TEXT,
            SPRITE
        };

        Type type;
        float x;
        float y;
        float width;
        float height;
        std::string text;
        std::vector<unsigned char> color; // RGBA
    };

    std::vector<Entity> entities;
    int score;
    std::string playerName;
};

class IGraphic {
public:
    using Ptr = std::unique_ptr<IGraphic, void(*)(IGraphic*)>;

    virtual ~IGraphic() = default;

    // Initialization and cleanup
    virtual bool initialize() = 0;
    virtual void cleanup() = 0;

    // Input handling
    virtual Input getInput() = 0;

    // Rendering
    virtual void render(const GameState& state) = 0;
    virtual void clear() = 0;
    virtual void display() = 0;

    // Window management
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual void setTitle(const std::string& title) = 0;

    // Player name input
    virtual void getPlayerName(std::string& playerName) = 0;

    // Library information
    virtual std::string getName() const = 0;
};

} // namespace arcd

// Factory functions
extern "C" {
    arcd::IGraphic* create_graphic();
    void destroy_graphic(arcd::IGraphic*);
} 