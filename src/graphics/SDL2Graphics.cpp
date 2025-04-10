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
    : _initialized(false), _width(80), _height(24), _lastKey(0), _frameCounter(0),
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
                             800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

    // Try multiple font paths
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
            _font = TTF_OpenFont(path.c_str(), 16);
            if (_font) {
                fontLoaded = true;
                break;
            }
        }
    }

    if (!fontLoaded) {
        std::cerr << "Failed to load any font. TTF_GetError: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(_renderer);
        SDL_DestroyWindow(_window);
        TTF_Quit();
        SDL_Quit();
        return false;
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
                default: return event.key.keysym.sym;
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