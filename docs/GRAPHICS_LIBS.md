# Graphics Libraries Documentation

This document details the graphics libraries in the Arcade project, explaining how they work, their interfaces, and implementation details.

## Overview

Graphics libraries in the Arcade project are responsible for:

1. **Rendering**: Displaying visual elements on screen
2. **Input Handling**: Capturing and processing user input
3. **Window Management**: Creating and managing application windows

Each graphics library implements the same interface (`IGraphicsLibrary`), allowing games to be rendered consistently regardless of which graphics library is active.

## Separation of Concerns

It's important to note that graphics libraries:
- Should NOT contain any game logic
- Should NOT make gameplay decisions
- Should ONLY render what's provided by the Core
- Should ONLY capture inputs and pass them to the Core without interpretation

## Available Graphics Libraries

The Arcade project includes several graphics libraries:

| Library | Technology | Features |
|---------|------------|----------|
| NCurses | Text-based terminal | Simple, works in terminal environments |
| SDL2 | Hardware-accelerated 2D | Fast rendering, modern features |
| SFML | Simple and Fast Multimedia Library | Object-oriented, modern features |

## Graphics Library Interface

All graphics libraries implement the `IGraphicsLibrary` interface:

```cpp
class IGraphicsLibrary {
public:
    virtual ~IGraphicsLibrary() = default;
    
    // Lifecycle management
    virtual bool initialize() = 0;
    virtual void cleanup() = 0;
    
    // Display management
    virtual void clear() = 0;
    virtual void refresh() = 0;
    
    // Game state rendering
    virtual void renderGameState(const IGameState& state) = 0;
    
    // UI rendering
    virtual void renderUI(const std::vector<UIElement>& elements) = 0;
    
    // Input handling
    virtual std::optional<std::unique_ptr<IEvent>> pollEvent() = 0;
    
    // Player interaction
    virtual void getPlayerName(std::string& playerName) = 0;
    
    // Window information
    virtual std::string getName() const = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
};
```

## Event Interface

The `IEvent` interface is used to represent user input in a library-agnostic way:

```cpp
class IEvent {
public:
    virtual ~IEvent() = default;
    
    virtual EventType getType() const = 0;
    virtual KeyCode getKeyCode() const = 0;
    virtual char getCharacter() const = 0;
    
    // Mouse event information (if applicable)
    virtual int getMouseX() const = 0;
    virtual int getMouseY() const = 0;
    virtual MouseButton getMouseButton() const = 0;
};
```

## Game State Rendering

Graphics libraries render game states provided by the Core:

```cpp
void NcursesGraphics::renderGameState(const IGameState& state) 
{
    // Clear the game area
    clear();
    
    // Draw a border around the game area
    int width = state.getWidth();
    int height = state.getHeight();
    drawBorder(0, 0, width+1, height+1);
    
    // Render each entity in the game state
    for (const auto& entity : state.getEntities()) {
        int x = entity.x;
        int y = entity.y;
        std::string symbol = entity.symbol;
        Color color = getColorFromName(entity.colorName);
        
        // Set the appropriate color
        attron(COLOR_PAIR(static_cast<int>(color)));
        
        // Draw the entity symbol
        mvprintw(y+1, x+1, "%s", symbol.c_str());
        
        // Reset color attributes
        attroff(COLOR_PAIR(static_cast<int>(color)));
    }
    
    // Draw the score
    mvprintw(height+2, 1, "Score: %d", state.getScore());
    
    // Draw game over message if needed
    if (state.isGameOver()) {
        const std::string& message = state.getMessage();
        int messageX = (width - message.length()) / 2;
        mvprintw(height/2, messageX+1, "%s", message.c_str());
    }
    
    // Refresh the display
    refresh();
}
```

## Input Processing

Graphics libraries capture input events and convert them to a generic format:

```cpp
std::optional<std::unique_ptr<IEvent>> SFMLGraphics::pollEvent() 
{
    sf::Event sfEvent;
    if (_window.pollEvent(sfEvent)) {
        switch (sfEvent.type) {
            case sf::Event::Closed:
                return std::make_unique<Event>(EventType::WINDOW_CLOSED);
                
            case sf::Event::KeyPressed: {
                KeyCode keyCode = KeyCode::UNKNOWN;
                
                // Map SF::Keyboard keys to our KeyCode enum
                switch (sfEvent.key.code) {
                    case sf::Keyboard::Up:    keyCode = KeyCode::UP; break;
                    case sf::Keyboard::Down:  keyCode = KeyCode::DOWN; break;
                    case sf::Keyboard::Left:  keyCode = KeyCode::LEFT; break;
                    case sf::Keyboard::Right: keyCode = KeyCode::RIGHT; break;
                    case sf::Keyboard::Return: keyCode = KeyCode::ENTER; break;
                    case sf::Keyboard::Escape: keyCode = KeyCode::ESC; break;
                    case sf::Keyboard::BackSpace: keyCode = KeyCode::BACKSPACE; break;
                    case sf::Keyboard::F1: keyCode = KeyCode::NEXT_LIB; break;
                    case sf::Keyboard::F2: keyCode = KeyCode::NEXT_GAME; break;
                    case sf::Keyboard::Q: keyCode = KeyCode::QUIT; break;
                    case sf::Keyboard::R: keyCode = KeyCode::RESTART; break;
                    default:
                        // For other keys, we can use their ASCII value
                        if (sfEvent.key.code >= 0 && sfEvent.key.code < 128) {
                            return std::make_unique<Event>(
                                EventType::KEY_PRESSED, 
                                KeyCode::UNKNOWN,
                                static_cast<char>(sfEvent.key.code)
                            );
                        }
                        break;
                }
                
                return std::make_unique<Event>(EventType::KEY_PRESSED, keyCode);
            }
                
            case sf::Event::TextEntered:
                if (sfEvent.text.unicode < 128) {
                    return std::make_unique<Event>(
                        EventType::TEXT_ENTERED,
                        KeyCode::UNKNOWN,
                        static_cast<char>(sfEvent.text.unicode)
                    );
                }
                break;
                
            default:
                break;
        }
    }
    
    return std::nullopt; // No event available
}
```

## UI Rendering

Graphics libraries provide a way to render UI elements like menus and information panels:

```cpp
void SDLGraphics::renderUI(const std::vector<UIElement>& elements)
{
    for (const auto& element : elements) {
        if (element.type == UIElementType::TEXT) {
            // Set the color based on selection state
            SDL_Color color = getSDLColor(element.selected ? Color::YELLOW : element.color);
            
            // Render the text
            renderText(element.x, element.y, element.text, color);
        }
        else if (element.type == UIElementType::RECT) {
            // Create a rectangle
            SDL_Rect rect = {
                element.x, element.y, 
                element.width, element.height
            };
            
            // Set the color
            SDL_Color color = getSDLColor(element.color);
            
            // Draw the rectangle
            SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
            SDL_RenderDrawRect(_renderer, &rect);
        }
    }
}
```

## Creating a New Graphics Library

To create a new graphics library:

1. Create a class that inherits from `IGraphicsLibrary`
2. Implement all required methods
3. Provide a way to manage a window, handle input, and render content
4. Export the required creation and destruction functions:

```cpp
extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary() {
        return std::make_unique<MyGraphicsLib>();
    }
    
    void destroyGraphicsLibrary([[maybe_unused]] arcd::IGraphicsLibrary* graphicsLib) {
        // With smart pointers, this is not needed anymore
    }
}
```

## Best Practices

When implementing a graphics library:

1. **Performance**: Use efficient rendering techniques
2. **Resource Management**: Clean up resources properly in the cleanup method
3. **Error Handling**: Gracefully handle initialization failures and runtime errors
4. **Input Mapping**: Provide consistent key mappings across libraries
5. **Separation of Concerns**: Never include game logic in the graphics library
6. **Adaptability**: Support different screen sizes and aspect ratios
7. **Fallbacks**: Use sensible defaults when resources are unavailable 