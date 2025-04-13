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

    _window = SDL_CreateWindow("Arcade", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                             1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!_window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
    if (!_renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(_window);
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    // Utiliser uniquement la police Arial du dossier assets
    std::string fontPath = "assets/fonts/Arial.ttf";
    
    // Vérifier si le fichier existe
    if (!std::filesystem::exists(fontPath)) {
        std::cerr << "Error: Font file not found: " << fontPath << std::endl;
        std::cerr << "SDL2 Graphics library cannot start without this font." << std::endl;
        SDL_DestroyRenderer(_renderer);
        SDL_DestroyWindow(_window);
        TTF_Quit();
        SDL_Quit();
        exit(84); // Quitter avec le code 84 si la police n'est pas trouvée
    }
    
    // Essayer de charger la police
    _font = TTF_OpenFont(fontPath.c_str(), 16);
    if (!_font) {
        std::cerr << "Error: Failed to load font: " << fontPath << std::endl;
        std::cerr << "SDL2 Graphics library cannot start without this font." << std::endl;
        SDL_DestroyRenderer(_renderer);
        SDL_DestroyWindow(_window);
        TTF_Quit();
        SDL_Quit();
        exit(84); // Quitter avec le code 84 si la police ne peut pas être chargée
    }

    initColors();
    _initialized = true;
    return true;
}

void SDL2Graphics::cleanup()
{
    if (_font) {
        TTF_CloseFont(_font);
        _font = nullptr;
    }
    if (_renderer) {
        SDL_DestroyRenderer(_renderer);
        _renderer = nullptr;
    }
    if (_window) {
        SDL_DestroyWindow(_window);
        _window = nullptr;
    }
    TTF_Quit();
    SDL_Quit();
    _initialized = false;
}

void SDL2Graphics::clear()
{
    if (!_initialized)
        return;
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    SDL_RenderClear(_renderer);
}

void SDL2Graphics::refresh()
{
    if (!_initialized)
        return;
    SDL_RenderPresent(_renderer);
    _frameCounter++;
}

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

void SDL2Graphics::drawText(int x, int y, const std::string& text, Color color)
{
    if (!_initialized || !_font)
        return;

    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_Surface* surface = TTF_RenderText_Blended(_font, text.c_str(), sdlColor);
    if (!surface)
        return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dest;
    dest.x = x * 10;  // Scale factor for text
    dest.y = y * 20;  // Scale factor for text
    dest.w = surface->w;
    dest.h = surface->h;

    SDL_RenderCopy(_renderer, texture, nullptr, &dest);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void SDL2Graphics::drawBox(int x, int y, int width, int height, Color color)
{
    if (!_initialized)
        return;

    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_SetRenderDrawColor(_renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);

    SDL_Rect rect;
    rect.x = x * 10;
    rect.y = y * 20;
    rect.w = width * 10;
    rect.h = height * 20;

    SDL_RenderDrawRect(_renderer, &rect);
}

void SDL2Graphics::drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color)
{
    if (!_initialized)
        return;

    for (size_t i = 0; i < items.size(); ++i) {
        Color itemColor = (i == static_cast<size_t>(selectedIndex)) ? Color::YELLOW : color;
        drawText(x, y + i, items[i], itemColor);
    }
}

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
                    // Pour les touches alphabétiques
                    if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z) {
                        // Gestion spéciale de R/r
                        if (event.key.keysym.sym == SDLK_r) {
                            return IGraphicsLibrary::KEY_RESTART_GAME;
                        }
                        return event.key.keysym.sym;
                    }
                    // Pour les touches numériques
                    else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) {
                        return event.key.keysym.sym;
                    }
                    return event.key.keysym.sym;
            }
        }
    }
    return 0;
}

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

std::string SDL2Graphics::getName() const
{
    return "SDL2";
}

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
    
    // Draw centered title
    drawTextCentered(3, title, Color::WHITE);
    
    // Calculate positions for menu boxes
    int boxWidth = 25; // Augmenter la largeur
    int boxHeight = 15; // Augmenter la hauteur
    int spacing = 4;
    int totalWidth = 3 * boxWidth + 2 * spacing;
    int startX = (_width - totalWidth) / 2;
    int startY = 8;
    
    // Préparer les noms simplifiés
    std::vector<std::string> displayGameNames;
    for (const auto& name : gameOptions) {
        std::string displayName = name;
        // Enlever le préfixe "arcade_" s'il existe
        size_t prefixPos = displayName.find("arcade_");
        if (prefixPos != std::string::npos) {
            displayName = displayName.substr(prefixPos + 7); // 7 est la longueur de "arcade_"
        }
        // Enlever l'extension ".so" s'il existe
        size_t extPos = displayName.find(".so");
        if (extPos != std::string::npos) {
            displayName = displayName.substr(0, extPos);
        }
        displayGameNames.push_back(displayName);
    }
    
    std::vector<std::string> displayGraphicNames;
    for (const auto& name : graphicOptions) {
        std::string displayName = name;
        // Enlever le préfixe "arcade_" s'il existe
        size_t prefixPos = displayName.find("arcade_");
        if (prefixPos != std::string::npos) {
            displayName = displayName.substr(prefixPos + 7); // 7 est la longueur de "arcade_"
        }
        // Enlever l'extension ".so" s'il existe
        size_t extPos = displayName.find(".so");
        if (extPos != std::string::npos) {
            displayName = displayName.substr(0, extPos);
        }
        displayGraphicNames.push_back(displayName);
    }
    
    // Draw game options box
    bool isGameBoxSelected = (selectedMenu == 0);
    Color gameBoxColor = isGameBoxSelected ? Color::YELLOW : Color::WHITE;
    drawBoxWithTitle(startX, startY, boxWidth, boxHeight, "Games", gameBoxColor);
    
    // Draw game options
    for (size_t i = 0; i < displayGameNames.size() && i < static_cast<size_t>(boxHeight - 3); i++) {
        Color itemColor = (isGameBoxSelected && static_cast<int>(i) == selectedGameIndex) 
                          ? Color::YELLOW : Color::WHITE;
        drawText(startX + 2, startY + 3 + static_cast<int>(i), displayGameNames[i], itemColor);
    }
    
    // Draw graphics options box
    bool isGraphicsBoxSelected = (selectedMenu == 1);
    Color graphicsBoxColor = isGraphicsBoxSelected ? Color::YELLOW : Color::WHITE;
    drawBoxWithTitle(startX + boxWidth + spacing, startY, boxWidth, boxHeight, "Graphics", graphicsBoxColor);
    
    // Draw graphics options
    for (size_t i = 0; i < displayGraphicNames.size() && i < static_cast<size_t>(boxHeight - 3); i++) {
        Color itemColor = (isGraphicsBoxSelected && static_cast<int>(i) == selectedGraphicIndex) 
                          ? Color::YELLOW : Color::WHITE;
        drawText(startX + boxWidth + spacing + 2, startY + 3 + static_cast<int>(i), 
                displayGraphicNames[i], itemColor);
    }
    
    // Draw player name box
    bool isPlayerBoxSelected = (selectedMenu == 2);
    Color playerBoxColor = isPlayerBoxSelected ? Color::YELLOW : Color::WHITE;
    drawBoxWithTitle(startX + 2 * (boxWidth + spacing), startY, boxWidth, boxHeight, "Player: " + playerName, playerBoxColor);
    
    // Draw player options
    std::vector<std::string> playerOptions = {"Change Nickname", "Leaderboard"};
    for (size_t i = 0; i < playerOptions.size(); i++) {
        Color itemColor = (isPlayerBoxSelected && static_cast<int>(i) == playerOptionSelected) 
                          ? Color::YELLOW : Color::WHITE;
        drawText(startX + 2 * (boxWidth + spacing) + 2, startY + 3 + static_cast<int>(i), 
                playerOptions[i], itemColor);
    }
    
    // Draw instructions with clear spacing
    int instructionY = startY + boxHeight + 2;
    
    // Augmenter la hauteur de la boîte d'instructions
    const int instructionBoxHeight = 180; // Augmentation significative

    // Draw instruction box with increased size
    SDL_SetRenderDrawColor(_renderer, 50, 50, 50, 150);
    SDL_Rect instructionBox = {
        static_cast<int>((startX - 2) * 10), 
        static_cast<int>(instructionY * 20 - 10),
        static_cast<int>((totalWidth + 4) * 10), 
        instructionBoxHeight
    };
    SDL_RenderFillRect(_renderer, &instructionBox);
    
    // Draw instructions with spacing
    std::vector<std::pair<std::string, std::string>> instructions = {
        {"SELECT", "Use TAB to switch boxes"},
        {"NAVIGATE", "Use ARROWS to navigate"},
        {"CONFIRM", "Press ENTER to select"},
        {"EXIT", "Press ESC to exit"}
    };
    
    // Augmenter l'espacement vertical entre les instructions
    const int instructionSpacing = 3; // Augmenter l'espacement vertical
    
    for (size_t i = 0; i < instructions.size(); i++) {
        int y = instructionY + 1 + i * instructionSpacing;
        // Utiliser drawText pour les labels au lieu de drawBoldText
        drawText(startX + 2, y, instructions[i].first + ":", Color::YELLOW);
        drawText(startX + 12, y, instructions[i].second, Color::WHITE);
    }
    
    refresh();
}

void SDL2Graphics::drawTextCentered(int y, const std::string& text, Color color)
{
    if (!_initialized || !_font)
        return;

    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_Surface* surface = TTF_RenderText_Blended(_font, text.c_str(), sdlColor);
    if (!surface)
        return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dest;
    int screen_width;
    SDL_GetRendererOutputSize(_renderer, &screen_width, nullptr);
    
    dest.x = (screen_width - surface->w) / 2;  // Center horizontally
    dest.y = y * 20;  // Scale factor for text
    dest.w = surface->w;
    dest.h = surface->h;

    SDL_RenderCopy(_renderer, texture, nullptr, &dest);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void SDL2Graphics::drawBoxWithTitle(int x, int y, int width, int height, const std::string& title, Color color)
{
    if (!_initialized)
        return;

    // Draw the box
    drawBox(x, y, width, height, color);
    
    // Draw the title centered
    if (_font && !title.empty()) {
        SDL_Color sdlColor = _colors[static_cast<int>(color)];
        SDL_Surface* surface = TTF_RenderText_Blended(_font, title.c_str(), sdlColor);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
            if (texture) {
                SDL_Rect dest;
                dest.x = (x * 10) + ((width * 10) - surface->w) / 2;  // Center in box
                dest.y = (y * 20) + 10;  // Near top of box
                dest.w = surface->w;
                dest.h = surface->h;
                
                SDL_RenderCopy(_renderer, texture, nullptr, &dest);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }
}

void SDL2Graphics::drawFilledBox(int x, int y, int width, int height, [[maybe_unused]] char fillChar, Color color)
{
    if (!_initialized)
        return;

    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_SetRenderDrawColor(_renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);

    SDL_Rect rect;
    rect.x = x * 10;
    rect.y = y * 20;
    rect.w = width * 10;
    rect.h = height * 20;

    SDL_RenderFillRect(_renderer, &rect);
}

void SDL2Graphics::drawHorizontalLine(int x, int y, int width, Color color)
{
    if (!_initialized)
        return;

    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_SetRenderDrawColor(_renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);
    
    SDL_RenderDrawLine(_renderer, x * 10, y * 20, (x + width) * 10, y * 20);
}

void SDL2Graphics::drawVerticalLine(int x, int y, int height, Color color)
{
    if (!_initialized)
        return;

    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_SetRenderDrawColor(_renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);
    
    SDL_RenderDrawLine(_renderer, x * 10, y * 20, x * 10, (y + height) * 20);
}

void SDL2Graphics::drawProgressBar(int x, int y, int width, int value, int maxValue, Color color)
{
    if (!_initialized)
        return;

    // Draw the border
    drawBox(x, y, width, 1, color);
    
    // Calculate the fill width
    int fillWidth = (width - 2) * value / maxValue;
    if (fillWidth < 0) fillWidth = 0;
    if (fillWidth > width - 2) fillWidth = width - 2;
    
    // Draw the fill
    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_SetRenderDrawColor(_renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);
    
    SDL_Rect fillRect;
    fillRect.x = (x + 1) * 10;
    fillRect.y = y * 20 + 2;
    fillRect.w = fillWidth * 10;
    fillRect.h = 16;  // Slightly less than the height of the box
    
    SDL_RenderFillRect(_renderer, &fillRect);
}

void SDL2Graphics::drawBoldText(int x, int y, const std::string& text, Color color)
{
    if (!_initialized || !_font)
        return;

    // Utiliser uniquement un simple drawText avec la couleur demandée
    // au lieu de dessiner le texte deux fois qui cause l'effet de doublon
    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_Surface* surface = TTF_RenderText_Blended(_font, text.c_str(), sdlColor);
    if (!surface)
        return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dest;
    dest.x = x * 10;
    dest.y = y * 20;
    dest.w = surface->w;
    dest.h = surface->h;

    SDL_RenderCopy(_renderer, texture, nullptr, &dest);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

} // namespace arcd

extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary()
    {
        return std::make_unique<arcd::SDL2Graphics>();
    }

    void destroyGraphicsLibrary([[maybe_unused]] arcd::IGraphicsLibrary* graphicsLib)
    {
        // With smart pointers, this function is not needed anymore
        // but we keep it for compatibility
    }
} 