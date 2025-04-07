# Graphics Libraries Documentation

This document details the graphics libraries in the Arcade project, explaining how they work, their interfaces, and implementation details.

## Overview

Graphics libraries in the Arcade project are responsible for:

1. **Rendering**: Displaying visual elements on screen
2. **Input Handling**: Capturing and processing user input
3. **Window Management**: Creating and managing application windows

Each graphics library implements the same interface (`IGraphicsLibrary`), allowing games to be rendered consistently regardless of which graphics library is active.

## Available Graphics Libraries

The Arcade project includes several graphics libraries:

| Library | Technology | Features |
|---------|------------|----------|
| NCurses | Text-based terminal | Simple, works in terminal environments |
| SDL2 | Hardware-accelerated 2D | Fast rendering, modern features |
| SFML | Simple and Fast Multimedia Library | Object-oriented, modern features |

## Graphics Library Interface

All graphics libraries implement the `IGraphicsLibrary` interface defined in `include/interfaces/IGraphicsLibrary.hpp`:

```cpp
class IGraphicsLibrary {
public:
    // Common key codes
    static const int KEY_UP_CODE = -1;
    static const int KEY_DOWN_CODE = -2;
    static const int KEY_LEFT_CODE = -3;
    static const int KEY_RIGHT_CODE = -4;
    static const int KEY_ENTER_CODE = -5;
    static const int KEY_ESC_CODE = -6;
    static const int KEY_BACKSPACE_CODE = -7;
    static const int KEY_NEXT_LIB_CODE = -8;
    static const int KEY_NEXT_GAME_CODE = -9;
    
    // Colors for rendering
    enum class Color {
        DEFAULT, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE
    };
    
    virtual ~IGraphicsLibrary() = default;
    
    // Initialization and cleanup
    virtual bool initialize() = 0;
    virtual void cleanup() = 0;
    
    // Display functions
    virtual void clear() = 0;
    virtual void refresh() = 0;
    
    // Drawing functions
    virtual void drawText(int x, int y, const std::string& text, Color color = Color::DEFAULT) = 0;
    virtual void drawBox(int x, int y, int width, int height, Color color = Color::DEFAULT) = 0;
    virtual void drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color = Color::DEFAULT) = 0;
    
    // Input handling
    virtual int getKey() = 0;
    
    // Player name input
    virtual void getPlayerName(std::string& playerName) = 0;
    
    // Library information
    virtual std::string getName() const = 0;
    
    // Window dimensions
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
};
```

## SFML Graphics Library Implementation

The SFML graphics library (`SFMLGraphics`) implements the graphics interface using the SFML library.

### Key Classes and Components

#### `SFMLGraphics` Class

The main class implementing the `IGraphicsLibrary` interface for SFML.

```cpp
class SFMLGraphics : public IGraphicsLibrary {
private:
    bool _initialized;
    int _width;
    int _height;
    int _lastKey;
    int _frameCounter;
    int _selectedMenuItem;
    sf::RenderWindow _window;
    sf::Font _font;
    std::map<int, sf::Color> _colors;
    
    // Drawing element storage
    std::vector<TextDrawElement> _textElements;
    std::vector<RectDrawElement> _rectElements;
    
    // Helper methods
    void redrawElements();
    void drawTestScreen();
    int handleKeyPress(sf::Keyboard::Key key);
    void initColors();
    // ...
    
public:
    SFMLGraphics();
    ~SFMLGraphics() override;
    
    // IGraphicsLibrary implementation
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
    int getWidth() const override;
    int getHeight() const override;
};
```

### Initialization Process

The SFML library initialization process:

1. Create an SFML window with default size (800x600)
2. Set framerate limit to 60 FPS
3. Load a font from several possible locations
4. Initialize the color mapping
5. Draw the test screen menu
6. Mark the library as initialized

```cpp
bool SFMLGraphics::initialize() {
    if (_initialized)
        return true;

    // Create window
    _window.create(sf::VideoMode(sf::Vector2u(_width, _height)), "Arcade", sf::Style::Default);
    _window.setFramerateLimit(60);

    // Load font
    std::vector<std::string> fontPaths = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
        "/usr/share/fonts/dejavu/DejaVuSansMono.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf",
        "/usr/share/fonts/liberation/LiberationMono-Regular.ttf"
    };

    bool fontLoaded = false;
    for (const auto& path : fontPaths) {
        if (std::filesystem::exists(path)) {
            if (_font.openFromFile(path)) {
                fontLoaded = true;
                break;
            }
        }
    }

    if (!fontLoaded) {
        std::cerr << "Failed to load any font." << std::endl;
        _window.close();
        return false;
    }

    initColors();
    
    drawTestScreen();
    
    _initialized = true;
    return true;
}
```

### Rendering Flow

The rendering process in SFML follows these steps:

1. **Clear**: The `clear()` method clears the screen with a background color
2. **Draw Elements**: Game calls various draw methods to add elements to the frame
3. **Refresh**: The `refresh()` method displays the frame and processes events

### Input Handling

Input handling in SFML:

1. **Event Polling**: `refresh()` polls for events using SFML's event system
2. **Key Mapping**: Keys are mapped from SFML key codes to Arcade key codes
3. **Input Processing**: `getKey()` returns the last pressed key or polls for new events

```cpp
int SFMLGraphics::handleKeyPress(sf::Keyboard::Key key) {
    const int menuItemCount = 4;
    
    switch (key) {
        case sf::Keyboard::Key::Up:
            _selectedMenuItem = (_selectedMenuItem > 0) ? _selectedMenuItem - 1 : menuItemCount - 1;
            return IGraphicsLibrary::KEY_UP_CODE;
            
        case sf::Keyboard::Key::Down:
            _selectedMenuItem = (_selectedMenuItem < menuItemCount - 1) ? _selectedMenuItem + 1 : 0;
            return IGraphicsLibrary::KEY_DOWN_CODE;
            
        case sf::Keyboard::Key::Enter:
            return IGraphicsLibrary::KEY_ENTER_CODE;
            
        case sf::Keyboard::Key::Escape:
            return IGraphicsLibrary::KEY_ESC_CODE;
            
        case sf::Keyboard::Key::Num9:
            return IGraphicsLibrary::KEY_NEXT_LIB_CODE;
            
        case sf::Keyboard::Key::Num7:
            return IGraphicsLibrary::KEY_NEXT_GAME_CODE;
            
        case sf::Keyboard::Key::Right:
            return IGraphicsLibrary::KEY_RIGHT_CODE;
            
        case sf::Keyboard::Key::Left:
            return IGraphicsLibrary::KEY_LEFT_CODE;
            
        default:
            return static_cast<int>(key);
    }
}
```

### Menu Rendering

The SFML library includes a menu system for navigation:

1. The menu is drawn using `drawTestScreen()`
2. Menu navigation uses the up/down keys to change selection
3. Selected items are highlighted in yellow
4. Additional information and instructions are shown at the bottom

```cpp
void SFMLGraphics::drawTestScreen() {
    // Clear existing elements
    _textElements.clear();
    _rectElements.clear();
    
    // Clear the window
    _window.clear(sf::Color(20, 20, 50));
    
    // Draw background, frame, title, etc.
    // ...
    
    // Draw menu items with selection
    std::vector<std::string> menuItems = {
        "Play Game",
        "Select Game",
        "Select Graphics",
        "Quit"
    };
    
    float menuY = 160.0f;
    for (size_t i = 0; i < menuItems.size(); i++) {
        sf::Text menuText(_font, menuItems[i], 20);
        menuText.setFillColor(static_cast<int>(i) == _selectedMenuItem ? sf::Color::Yellow : sf::Color::White);
        
        // Center the menu item
        sf::FloatRect menuRect = menuText.getLocalBounds();
        centerX = (_width - menuRect.size.x) / 2.0f;
        menuText.setPosition(sf::Vector2f(centerX, menuY + i * 40.0f));
        _window.draw(menuText);
        
        // Add a selection indicator for the selected item
        if (static_cast<int>(i) == _selectedMenuItem) {
            sf::Text selectorText(_font, "> ", 20);
            selectorText.setFillColor(sf::Color::Yellow);
            selectorText.setPosition(sf::Vector2f(centerX - 30.0f, menuY + i * 40.0f));
            _window.draw(selectorText);
        }
    }
    
    // Draw instructions and additional information
    // ...
    
    // Display the frame
    _window.display();
}
```

## Memory Management

Graphics libraries manage resources like:
- Window handles
- Font resources
- Textures and sprites
- Drawing element collections

Resources are cleaned up in the `cleanup()` method and the destructor to prevent memory leaks.

## External Entry Points

Each graphics library provides C-style functions for dynamic loading:

```cpp
extern "C" {
    arcd::IGraphicsLibrary* createGraphicsLibrary() {
        return new arcd::SFMLGraphics();
    }

    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* graphicsLib) {
        delete graphicsLib;
    }
}
```

## Performance Considerations

For optimal performance, graphics libraries:
- Minimize redrawing when not needed
- Cache resources where possible
- Leverage hardware acceleration when available
- Maintain consistent frame rates

## Next Steps

For more information on related components:
- [Core Engine Documentation](CORE.md)
- [Game Libraries Documentation](GAME_LIBS.md)
- [Interfaces Documentation](INTERFACES.md) 