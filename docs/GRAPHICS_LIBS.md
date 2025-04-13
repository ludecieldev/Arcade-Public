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

| Library | Description | Key Features |
|---------|-------------|--------------|
| Ncurses | Terminal-based graphics | Lightweight, no dependencies |
| SDL2 | Simple DirectMedia Layer | Cross-platform, hardware acceleration |
| SFML | Simple and Fast Multimedia Library | Object-oriented, feature-rich |

## Library Loading and Management

### Loading Process

The loading of graphics libraries follows these critical steps:

1. **Initialization** - The library is loaded via dlopen and symbols are retrieved
2. **Creation** - The createGraphicsLibrary() function is called to instantiate the library
3. **Setup** - The library's initialize() method is called to set up resources

### Unloading Process

Unloading libraries follows a careful sequence to prevent crashes:

1. **Cleanup** - Call cleanup() to free resources managed by the library
2. **Anti-crash delay** - Wait 100ms to ensure resources are fully released
3. **Unload** - Unload the library with dlclose()
4. **Final delay** - Wait 50ms to stabilize the system state

### Special Handling for NCurses

NCurses requires special treatment to avoid segmentation faults:

1. **Pointer detachment** - Use `.release()` instead of `reset()` to avoid calling the destructor
2. **Extended delays** - Use longer delays (150ms) after cleanup
3. **Resource check** - Special handling in hasGraphicsLibrary() to detect released NCurses instances

```cpp
// Special handling for NCurses
if (isNcurses) {
    _currentGraphicsLib.release(); // Detach pointer without destroying
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
}
```

### Switching Libraries

When switching between graphics libraries:

1. **State preservation** - Save important state (player name, game state)
2. **Safe unloading** - Unload current library with proper delay sequence
3. **Loading** - Load the new library with proper error handling
4. **Delay** - Wait 50ms before initializing the new library
5. **State restoration** - Restore saved state to the new library

### Anti-Crash Timing Strategy

The system employs strategic delays to prevent segfaults:

| Operation | Delay | Purpose |
|-----------|-------|---------|
| After cleanup() | 100ms | Allow resources to be properly released |
| Before initialize() | 50ms | Ensure system stability before initialization |
| After unload | 50ms | Prevent race conditions |
| NCurses specific | 150ms | Extra time for terminal resources |

## Graphics Library Interface

All graphics libraries implement the `IGraphicsLibrary` interface defined in `include/interfaces/IGraphicsLibrary.hpp`:

```cpp
class IGraphicsLibrary {
public:
    // Common key codes
    static constexpr int KEY_UP_CODE = 259;
    static constexpr int KEY_DOWN_CODE = 258;
    static constexpr int KEY_LEFT_CODE = 260;
    static constexpr int KEY_RIGHT_CODE = 261;
    static constexpr int KEY_ENTER_CODE = 10;
    static constexpr int KEY_ESC_CODE = 27;
    static constexpr int KEY_BACKSPACE_CODE = 127;
    static constexpr int KEY_NEXT_LIB_CODE = '9';
    static constexpr int KEY_NEXT_GAME_CODE = '7';
    static constexpr int KEY_SPACE_CODE = ' ';
    static constexpr int KEY_TAB_CODE = '\t';
    static constexpr int KEY_RESTART_GAME = 'r';
    
    // Mouse constants
    static constexpr int MOUSE_LEFT_BUTTON = 1;
    static constexpr int MOUSE_RIGHT_BUTTON = 2;
    static constexpr int MOUSE_MIDDLE_BUTTON = 3;
    
    // Mouse event structure
    struct MouseEvent {
        int x;                // X coordinate
        int y;                // Y coordinate
        int button;           // Button code (MOUSE_LEFT_BUTTON, MOUSE_RIGHT_BUTTON, etc.)
        bool pressed;         // true if button is pressed, false if released
        bool hasEvent;        // true if there's a mouse event, false otherwise
    };
    
    // Colors for rendering
    enum class Color {
        DEFAULT, BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE
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
    
    // Standardized menu drawing
    virtual void drawMenu(
        const std::string& title,
        const std::vector<std::string>& gameOptions,
        const std::vector<std::string>& graphicOptions,
        const std::string& playerName,
        int selectedMenu,
        int selectedGameIndex,
        int selectedGraphicIndex) = 0;
    
    // Input handling
    virtual int getKey() = 0;
    virtual MouseEvent getMouse() = 0;
    
    // Player name input
    virtual void getPlayerName(std::string& playerName) = 0;
    
    // Library information
    virtual std::string getName() const = 0;
    
    // Window dimensions
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
};
```

## Ncurses Graphics Library Implementation

The Ncurses graphics library (`NcursesGraphics`) implements the graphics interface using the Ncurses library.

### Features
- Terminal-based graphics
- Color support
- Keyboard input handling
- Mouse input handling (basic support)
- Simple text and box drawing

### Special Considerations
- **Resource Management**: Requires special handling to avoid segfaults when unloading
- **Terminal Reset**: Needs proper terminal state restoration in cleanup()
- **Timing**: Needs longer delays when unloading to avoid crashes
- **Mouse Support**: Basic mouse support with position and button detection

## SDL2 Graphics Library Implementation

The SDL2 graphics library (`SDL2Graphics`) implements the graphics interface using the SDL2 library.

### Features
- Window-based graphics
- Hardware acceleration
- TrueType font support
- Event handling
- Color management
- Full mouse support with coordinates and buttons

## SFML Graphics Library Implementation

The SFML graphics library (`SFMLGraphics`) implements the graphics interface using the SFML library.

### Features
- Object-oriented graphics API
- Multiple window support
- Event handling
- Font rendering
- Color management

## Allegro5 Graphics Library Implementation

The Allegro5 graphics library (`Allegro5Graphics`) implements the graphics interface using the Allegro5 library.

### Features
- Window-based graphics
- Hardware acceleration
- TrueType font support
- Event handling
- Color management
- Full mouse support with coordinates and buttons
- Coordinate scaling for consistent display across libraries

## Memory Management Practices

To ensure stability when loading/unloading libraries, follow these practices:

1. **Resource Cleanup**: Always call cleanup() before unloading
2. **Strategic Delays**: Use appropriate delays between operations
3. **Special Cases**: Handle NCurses differently from window-based libraries
4. **Error Recovery**: Always attempt to restore the previous library if loading fails
5. **State Preservation**: Save and restore important state when switching libraries

## Troubleshooting Library Loading

### Common Issues

1. **Segmentation Faults**: 
   - Usually caused by improper unloading sequence
   - Solution: Use strategic delays and proper cleanup sequence

2. **Resource Leaks**:
   - Caused by failure to call cleanup() or improper cleanup
   - Solution: Ensure all resources are freed in cleanup()

3. **NCurses-Specific Crashes**:
   - Caused by destructor issues with terminal resources
   - Solution: Use release() instead of reset(), add extra delays

4. **Failed Library Loading**:
   - Caused by missing dependencies or incompatible libraries
   - Solution: Verify dependencies, check error messages from dlopen

## Factory Function

Each graphics library provides a factory function for dynamic loading:

```cpp
extern "C" std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary() {
    return std::make_unique<arcd::SFMLGraphics>();
}
```

## Performance Considerations

For optimal performance, graphics libraries:
- Minimize redrawing when not needed
- Cache resources where possible
- Leverage hardware acceleration when available
- Maintain consistent frame rates

## Input Handling

### Keyboard Input

All graphics libraries standardize keyboard input through the `getKey()` method, which returns standardized key codes.

### Mouse Input

The Arcade project includes mouse input support through the `getMouse()` method. This returns a `MouseEvent` structure containing:

- **x, y**: The mouse cursor coordinates
- **button**: The button that was pressed/released (LEFT, RIGHT, MIDDLE)
- **pressed**: Whether the button is being pressed (true) or released (false)
- **hasEvent**: Whether a mouse event has occurred

Mouse input is standardized across all graphics libraries:

| Library | Mouse Support Level |
|---------|---------------------|
| Ncurses | Basic support (terminal limitations) |
| SDL2    | Full support with precise coordinates |
| Allegro5| Full support with precise coordinates |

Games implement the `handleMouseInput()` method to respond to mouse events, which is particularly useful for games like Minesweeper that benefit from mouse interaction.

## Coordinate Scaling

To ensure consistent display across different graphics libraries, coordinate scaling is implemented:

- **Ncurses**: Uses character-based coordinates (1 unit = 1 character cell)
- **SDL2**: Applies scaling factors (typically 10x for x-coordinate, 20x for y-coordinate)
- **Allegro5**: Applies similar scaling factors as SDL2 for consistency

This scaling ensures that games appear properly sized regardless of which graphics library is in use.

## Next Steps

For more information on related components:
- [Core Engine Documentation](CORE.md)
- [Game Libraries Documentation](GAME_LIBS.md)
- [Interfaces Documentation](INTERFACES.md) 