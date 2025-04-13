/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** SDL2Graphics implementation
*/

#include "../../include/graphics/SDL2Graphics.hpp"
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <filesystem>

namespace arcd {

SDL2Graphics::SDL2Graphics()
    : _initialized(false), _width(100), _height(50), _lastKey(0), _frameCounter(0),
      _window(nullptr), _renderer(nullptr), _font(nullptr)
{
}

SDL2Graphics::~SDL2Graphics()
{
    cleanup();
}

/**
 * @brief Initialize the graphics library
 * - Check if already initialized
 * - Initialize SDL2 and SDL_ttf
 * - Create window and renderer
 * - Load required font
 * - Initialize colors
 * 
 * @return true if initialization succeeded, false otherwise
 */
bool SDL2Graphics::initialize()
{
    if (_initialized)
        return true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() < 0) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    _window.reset(SDL_CreateWindow("Arcade", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                             1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));
    if (!_window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    _renderer.reset(SDL_CreateRenderer(_window.get(), -1, SDL_RENDERER_ACCELERATED));
    if (!_renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        _window.reset();
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    std::string fontPath = "assets/fonts/Arial.ttf";
    
    if (!std::filesystem::exists(fontPath)) {
        std::cerr << "Error: Font file not found: " << fontPath << std::endl;
        std::cerr << "SDL2 Graphics library cannot start without this font." << std::endl;
        _renderer.reset();
        _window.reset();
        TTF_Quit();
        SDL_Quit();
        exit(84);
    }
    
    _font.reset(TTF_OpenFont(fontPath.c_str(), 16));
    if (!_font) {
        std::cerr << "Error: Failed to load font: " << fontPath << std::endl;
        std::cerr << "SDL2 Graphics library cannot start without this font." << std::endl;
        _renderer.reset();
        _window.reset();
        TTF_Quit();
        SDL_Quit();
        exit(84);
    }

    initColors();
    _initialized = true;
    return true;
}

/**
 * @brief Clean up resources used by the graphics library
 * - Free font resources
 * - Destroy renderer and window
 * - Quit SDL_ttf and SDL
 */
void SDL2Graphics::cleanup()
{
    _font.reset();
    _renderer.reset();
    _window.reset();
    TTF_Quit();
    SDL_Quit();
    _initialized = false;
}

/**
 * @brief Clear the screen to black
 */
void SDL2Graphics::clear()
{
    if (!_initialized)
        return;
    SDL_SetRenderDrawColor(_renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(_renderer.get());
}

/**
 * @brief Refresh the display (swap buffers)
 * - Present renderer
 * - Increment frame counter
 */
void SDL2Graphics::refresh()
{
    if (!_initialized)
        return;
    SDL_RenderPresent(_renderer.get());
    _frameCounter++;
}

/**
 * @brief Initialize color mappings for the library
 * - Map each Color enum value to an SDL_Color
 */
void SDL2Graphics::initColors()
{
    _colors[static_cast<int>(Color::DEFAULT)] = {255, 255, 255, 255};
    _colors[static_cast<int>(Color::RED)] = {255, 0, 0, 255};
    _colors[static_cast<int>(Color::GREEN)] = {0, 255, 0, 255};
    _colors[static_cast<int>(Color::YELLOW)] = {255, 255, 0, 255};
    _colors[static_cast<int>(Color::BLUE)] = {0, 0, 255, 255};
    _colors[static_cast<int>(Color::MAGENTA)] = {255, 0, 255, 255};
    _colors[static_cast<int>(Color::CYAN)] = {0, 255, 255, 255};
    _colors[static_cast<int>(Color::WHITE)] = {255, 255, 255, 255};
}

/**
 * @brief Draw text at the specified position
 * - Create surface from text
 * - Create texture from surface
 * - Render texture at specified position
 * - Clean up resources
 * 
 * @param x x-coordinate
 * @param y y-coordinate
 * @param text the text to draw
 * @param color color to use for drawing
 */
void SDL2Graphics::drawText(int x, int y, const std::string& text, Color color)
{
    if (!_initialized || !_font)
        return;

    auto surface = createTextSurface(text, color);
    if (!surface)
        return;

    auto texture = createTextureFromSurface(surface.get());
    if (!texture)
        return;

    SDL_Rect dest;
    dest.x = x * 10;
    dest.y = y * 20;
    dest.w = surface->w;
    dest.h = surface->h;

    SDL_RenderCopy(_renderer.get(), texture.get(), nullptr, &dest);
}

/**
 * @brief Draw a box at the specified position
 * - Set render draw color based on specified color
 * - Create rectangle with proper dimensions
 * - Draw rectangle outline
 * 
 * @param x x-coordinate
 * @param y y-coordinate
 * @param width width of the box
 * @param height height of the box
 * @param color color to use for drawing
 */
void SDL2Graphics::drawBox(int x, int y, int width, int height, Color color)
{
    if (!_initialized)
        return;

    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_SetRenderDrawColor(_renderer.get(), sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);

    SDL_Rect rect;
    rect.x = x * 10;
    rect.y = y * 20;
    rect.w = width * 10;
    rect.h = height * 20;

    SDL_RenderDrawRect(_renderer.get(), &rect);
}

/**
 * @brief Draw a list of items with a selected index
 * - Use different colors for selected and non-selected items
 * - Draw each item at the appropriate position
 * 
 * @param x x-coordinate
 * @param y y-coordinate
 * @param items vector of strings to display
 * @param selectedIndex index of the selected item
 * @param color color to use for drawing
 */
void SDL2Graphics::drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color)
{
    if (!_initialized)
        return;

    for (size_t i = 0; i < items.size(); ++i) {
        Color itemColor = (i == static_cast<size_t>(selectedIndex)) ? Color::YELLOW : color;
        drawText(x, y + i, items[i], itemColor);
    }
}

/**
 * @brief Get the last key pressed
 * - Check if initialized
 * - Process SDL events
 * - Map SDL key codes to application key codes
 * 
 * @return integer code of the key or 0 if no key pressed
 */
int SDL2Graphics::getKey()
{
    if (!_initialized)
        return 0;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return IGraphicsLibrary::KEY_ESC_CODE;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP: return IGraphicsLibrary::KEY_UP_CODE;
                case SDLK_DOWN: return IGraphicsLibrary::KEY_DOWN_CODE;
                case SDLK_RIGHT: return IGraphicsLibrary::KEY_RIGHT_CODE;
                case SDLK_LEFT: return IGraphicsLibrary::KEY_LEFT_CODE;
                case SDLK_RETURN: return IGraphicsLibrary::KEY_ENTER_CODE;
                case SDLK_ESCAPE: return IGraphicsLibrary::KEY_ESC_CODE;
                case SDLK_BACKSPACE: return IGraphicsLibrary::KEY_BACKSPACE_CODE;
                case SDLK_9: return IGraphicsLibrary::KEY_NEXT_LIB_CODE;
                case SDLK_7: return IGraphicsLibrary::KEY_NEXT_GAME_CODE;
                case SDLK_r: return IGraphicsLibrary::KEY_RESTART_GAME;
                default: 
                    if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z) {
                        if (event.key.keysym.sym == SDLK_r) {
                            return IGraphicsLibrary::KEY_RESTART_GAME;
                        }
                        return event.key.keysym.sym;
                    }
                    else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) {
                        return event.key.keysym.sym;
                    }
                    return event.key.keysym.sym;
            }
        }
    }
    return 0;
}

/**
 * @brief Get the player's name via user input
 * - Display prompt for player name
 * - Process keyboard events
 * - Handle backspace, enter, and escape keys
 * - Handle regular character input
 * 
 * @param playerName reference to string to store the player name
 */
void SDL2Graphics::getPlayerName(std::string& playerName)
{
    if (!_initialized)
        return;

    playerName.clear();
    bool done = false;
    SDL_Event event;

    while (!done) {
        clear();
        drawText(10, 10, "Enter your name: " + playerName + "_", Color::WHITE);
        refresh();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = true;
                break;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN && !playerName.empty()) {
                    done = true;
                } else if (event.key.keysym.sym == SDLK_BACKSPACE && !playerName.empty()) {
                    playerName.pop_back();
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    playerName.clear();
                    done = true;
                } else if (playerName.length() < 20) {
                    char c = event.key.keysym.sym;
                    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
                        playerName += c;
                    }
                }
            }
        }
    }
}

/**
 * @brief Get the name of the graphics library
 * 
 * @return name of the graphics library
 */
std::string SDL2Graphics::getName() const
{
    return "SDL2";
}

/**
 * @brief Draw the menu screen
 * - Clear the screen
 * - Draw centered title
 * - Calculate positions for menu boxes
 * - Process game and graphics library names for display
 * - Draw game options box and items
 * - Draw graphics options box and items
 * - Draw player options box and items
 * - Draw instructions box with controls
 * - Refresh the display
 * 
 * @param title title of the menu
 * @param gameOptions list of game options
 * @param graphicOptions list of graphic library options
 * @param playerName name of the player
 * @param selectedMenu selected menu section
 * @param selectedGameIndex selected game index
 * @param selectedGraphicIndex selected graphics library index
 * @param playerOptionSelected selected player option index
 */
void SDL2Graphics::drawMenu(
    const std::string& title,
    const std::vector<std::string>& gameOptions,
    const std::vector<std::string>& graphicOptions,
    const std::string& playerName,
    int selectedMenu,
    int selectedGameIndex,
    int selectedGraphicIndex,
    int playerOptionSelected)
{
    if (!_initialized) return;
    
    clear();
    
    drawTextCentered(3, title, Color::WHITE);
    
    int boxWidth = 25;
    int boxHeight = 15;
    int spacing = 4;
    int totalWidth = 3 * boxWidth + 2 * spacing;
    int startX = (_width - totalWidth) / 2;
    int startY = 8;
    
    std::vector<std::string> displayGameNames;
    for (const auto& name : gameOptions) {
        std::string displayName = name;
        size_t prefixPos = displayName.find("arcade_");
        if (prefixPos != std::string::npos) {
            displayName = displayName.substr(prefixPos + 7);
        }
        size_t extPos = displayName.find(".so");
        if (extPos != std::string::npos) {
            displayName = displayName.substr(0, extPos);
        }
        displayGameNames.push_back(displayName);
    }
    
    std::vector<std::string> displayGraphicNames;
    for (const auto& name : graphicOptions) {
        std::string displayName = name;
        size_t prefixPos = displayName.find("arcade_");
        if (prefixPos != std::string::npos) {
            displayName = displayName.substr(prefixPos + 7);
        }
        size_t extPos = displayName.find(".so");
        if (extPos != std::string::npos) {
            displayName = displayName.substr(0, extPos);
        }
        displayGraphicNames.push_back(displayName);
    }
    
    bool isGameBoxSelected = (selectedMenu == 0);
    Color gameBoxColor = isGameBoxSelected ? Color::YELLOW : Color::WHITE;
    drawBoxWithTitle(startX, startY, boxWidth, boxHeight, "Games", gameBoxColor);
    
    for (size_t i = 0; i < displayGameNames.size() && i < static_cast<size_t>(boxHeight - 3); i++) {
        Color itemColor = (isGameBoxSelected && static_cast<int>(i) == selectedGameIndex) 
                          ? Color::YELLOW : Color::WHITE;
        drawText(startX + 2, startY + 3 + static_cast<int>(i), displayGameNames[i], itemColor);
    }
    
    bool isGraphicsBoxSelected = (selectedMenu == 1);
    Color graphicsBoxColor = isGraphicsBoxSelected ? Color::YELLOW : Color::WHITE;
    drawBoxWithTitle(startX + boxWidth + spacing, startY, boxWidth, boxHeight, "Graphics", graphicsBoxColor);
    
    for (size_t i = 0; i < displayGraphicNames.size() && i < static_cast<size_t>(boxHeight - 3); i++) {
        Color itemColor = (isGraphicsBoxSelected && static_cast<int>(i) == selectedGraphicIndex) 
                          ? Color::YELLOW : Color::WHITE;
        drawText(startX + boxWidth + spacing + 2, startY + 3 + static_cast<int>(i), 
                displayGraphicNames[i], itemColor);
    }
    
    bool isPlayerBoxSelected = (selectedMenu == 2);
    Color playerBoxColor = isPlayerBoxSelected ? Color::YELLOW : Color::WHITE;
    drawBoxWithTitle(startX + 2 * (boxWidth + spacing), startY, boxWidth, boxHeight, "Player: " + playerName, playerBoxColor);
    
    std::vector<std::string> playerOptions = {"Change Nickname", "Leaderboard"};
    for (size_t i = 0; i < playerOptions.size(); i++) {
        Color itemColor = (isPlayerBoxSelected && static_cast<int>(i) == playerOptionSelected) 
                          ? Color::YELLOW : Color::WHITE;
        drawText(startX + 2 * (boxWidth + spacing) + 2, startY + 3 + static_cast<int>(i), 
                playerOptions[i], itemColor);
    }
    
    int instructionY = startY + boxHeight + 2;
    
    const int instructionBoxHeight = 180;

    SDL_SetRenderDrawColor(_renderer.get(), 50, 50, 50, 150);
    SDL_Rect instructionBox = {
        static_cast<int>((startX - 2) * 10), 
        static_cast<int>(instructionY * 20 - 10),
        static_cast<int>((totalWidth + 4) * 10), 
        instructionBoxHeight
    };
    SDL_RenderFillRect(_renderer.get(), &instructionBox);
    
    std::vector<std::pair<std::string, std::string>> instructions = {
        {"SELECT", "Use TAB to switch boxes"},
        {"NAVIGATE", "Use ARROWS to navigate"},
        {"CONFIRM", "Press ENTER to select"},
        {"EXIT", "Press ESC to exit"}
    };
    
    const int instructionSpacing = 3;
    
    for (size_t i = 0; i < instructions.size(); i++) {
        int y = instructionY + 1 + i * instructionSpacing;
        drawText(startX + 2, y, instructions[i].first + ":", Color::YELLOW);
        drawText(startX + 12, y, instructions[i].second, Color::WHITE);
    }
    
    refresh();
}

/**
 * @brief Draw text centered horizontally at the specified y position
 * - Create surface from text
 * - Create texture from surface
 * - Calculate horizontal center position
 * - Render texture at centered position
 * - Clean up resources
 * 
 * @param y y-coordinate
 * @param text the text to draw
 * @param color color to use for drawing
 */
void SDL2Graphics::drawTextCentered(int y, const std::string& text, Color color)
{
    if (!_initialized || !_font)
        return;

    auto surface = createTextSurface(text, color);
    if (!surface)
        return;

    auto texture = createTextureFromSurface(surface.get());
    if (!texture)
        return;

    SDL_Rect dest;
    int screen_width;
    SDL_GetRendererOutputSize(_renderer.get(), &screen_width, nullptr);
    
    dest.x = (screen_width - surface->w) / 2;
    dest.y = y * 20;
    dest.w = surface->w;
    dest.h = surface->h;

    SDL_RenderCopy(_renderer.get(), texture.get(), nullptr, &dest);
}

/**
 * @brief Draw a box with a title
 * - Draw the box outline
 * - Create text surface and texture
 * - Calculate position to center title
 * - Render title
 * - Clean up resources
 * 
 * @param x x-coordinate
 * @param y y-coordinate
 * @param width width of the box
 * @param height height of the box
 * @param title title text for the box
 * @param color color to use for drawing
 */
void SDL2Graphics::drawBoxWithTitle(int x, int y, int width, int height, const std::string& title, Color color)
{
    if (!_initialized)
        return;
    drawBox(x, y, width, height, color);
    
    if (_font && !title.empty()) {
        auto surface = createTextSurface(title, color);
        if (!surface)
            return;
            
        auto texture = createTextureFromSurface(surface.get());
        if (!texture)
            return;
            
        SDL_Rect dest;
        dest.x = (x * 10) + ((width * 10) - surface->w) / 2;
        dest.y = (y * 20) + 10;
        dest.w = surface->w;
        dest.h = surface->h;
        
        SDL_RenderCopy(_renderer.get(), texture.get(), nullptr, &dest);
    }
}

/**
 * @brief Draw a filled box
 * - Set render draw color based on specified color
 * - Create rectangle with proper dimensions
 * - Fill the rectangle
 * 
 * @param x x-coordinate
 * @param y y-coordinate
 * @param width width of the box
 * @param height height of the box
 * @param fillChar character to fill the box with (unused in SDL implementation)
 * @param color color to use for drawing
 */
void SDL2Graphics::drawFilledBox(int x, int y, int width, int height, [[maybe_unused]] char fillChar, Color color)
{
    if (!_initialized)
        return;

    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_SetRenderDrawColor(_renderer.get(), sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);

    SDL_Rect rect;
    rect.x = x * 10;
    rect.y = y * 20;
    rect.w = width * 10;
    rect.h = height * 20;

    SDL_RenderFillRect(_renderer.get(), &rect);
}

/**
 * @brief Draw a horizontal line
 * - Set render draw color based on specified color
 * - Draw line from start position with specified width
 * 
 * @param x x-coordinate of start
 * @param y y-coordinate
 * @param width length of the line
 * @param color color to use for drawing
 */
void SDL2Graphics::drawHorizontalLine(int x, int y, int width, Color color)
{
    if (!_initialized)
        return;

    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_SetRenderDrawColor(_renderer.get(), sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);
    
    SDL_RenderDrawLine(_renderer.get(), x * 10, y * 20, (x + width) * 10, y * 20);
}

/**
 * @brief Draw a vertical line
 * - Set render draw color based on specified color
 * - Draw line from start position with specified height
 * 
 * @param x x-coordinate
 * @param y y-coordinate of start
 * @param height height of the line
 * @param color color to use for drawing
 */
void SDL2Graphics::drawVerticalLine(int x, int y, int height, Color color)
{
    if (!_initialized)
        return;

    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_SetRenderDrawColor(_renderer.get(), sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);
    
    SDL_RenderDrawLine(_renderer.get(), x * 10, y * 20, x * 10, (y + height) * 20);
}

/**
 * @brief Draw a progress bar
 * - Draw the border
 * - Calculate the fill width based on value and maximum
 * - Set render draw color
 * - Fill the progress bar to appropriate level
 * 
 * @param x x-coordinate
 * @param y y-coordinate
 * @param width width of the progress bar
 * @param value current value
 * @param maxValue maximum value
 * @param color color to use for drawing
 */
void SDL2Graphics::drawProgressBar(int x, int y, int width, int value, int maxValue, Color color)
{
    if (!_initialized)
        return;
    drawBox(x, y, width, 1, color);
    
    int fillWidth = (width - 2) * value / maxValue;
    if (fillWidth < 0) fillWidth = 0;
    if (fillWidth > width - 2) fillWidth = width - 2;
    
    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_SetRenderDrawColor(_renderer.get(), sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);
    
    SDL_Rect fillRect;
    fillRect.x = (x + 1) * 10;
    fillRect.y = y * 20 + 2;
    fillRect.w = fillWidth * 10;
    fillRect.h = 16;
    
    SDL_RenderFillRect(_renderer.get(), &fillRect);
}

/**
 * @brief Draw bold text at the specified position
 * - Create surface from text
 * - Create texture from surface
 * - Render texture at specified position
 * - Clean up resources
 * 
 * @param x x-coordinate
 * @param y y-coordinate
 * @param text the text to draw
 * @param color color to use for drawing
 */
void SDL2Graphics::drawBoldText(int x, int y, const std::string& text, Color color)
{
    if (!_initialized || !_font)
        return;

    auto surface = createTextSurface(text, color);
    if (!surface)
        return;

    auto texture = createTextureFromSurface(surface.get());
    if (!texture)
        return;

    SDL_Rect dest;
    dest.x = x * 10;
    dest.y = y * 20;
    dest.w = surface->w;
    dest.h = surface->h;

    SDL_RenderCopy(_renderer.get(), texture.get(), nullptr, &dest);
}

std::unique_ptr<SDL_Surface, SDL2Graphics::SDLDeleter> 
SDL2Graphics::createTextSurface(const std::string& text, Color color)
{
    if (!_initialized || !_font || text.empty())
        return nullptr;

    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    return std::unique_ptr<SDL_Surface, SDLDeleter>(
        TTF_RenderText_Blended(_font.get(), text.c_str(), sdlColor)
    );
}

std::unique_ptr<SDL_Texture, SDL2Graphics::SDLDeleter>
SDL2Graphics::createTextureFromSurface(SDL_Surface* surface)
{
    if (!_initialized || !_renderer || !surface)
        return nullptr;

    return std::unique_ptr<SDL_Texture, SDLDeleter>(
        SDL_CreateTextureFromSurface(_renderer.get(), surface)
    );
}

} // namespace arcd

extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary()
    {
        return std::make_unique<arcd::SDL2Graphics>();
    }
}