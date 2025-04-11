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
#include <chrono>
#include <thread>
#include <sstream>

namespace arcd {

SDL2Graphics::SDL2Graphics()
    : _initialized(false), _width(800), _height(600), _lastKey(0), _frameCounter(0),
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

    _window = SDL_CreateWindow("Arcade - SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              _width, _height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

    _font = TTF_OpenFont("assets/fonts/DejaVuSansMono.ttf", 14);
    if (!_font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        _font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSansMono.ttf", 14);
        if (!_font) {
            std::cerr << "Failed to load fallback font! TTF_Error: " << TTF_GetError() << std::endl;
            SDL_DestroyRenderer(_renderer);
            SDL_DestroyWindow(_window);
            TTF_Quit();
            SDL_Quit();
            return false;
        }
    }

    initColors();
    updateWindowSize();
    _initialized = true;
    showSplashScreen();
    return true;
}

void SDL2Graphics::cleanup()
{
    if (!_initialized)
        return;

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

void SDL2Graphics::updateWindowSize()
{
    if (!_initialized)
        return;

    SDL_GetWindowSize(_window, &_width, &_height);
}

void SDL2Graphics::initColors()
{
    _colors[static_cast<int>(Color::DEFAULT)] = {255, 255, 255, 255};
    _colors[static_cast<int>(Color::BLACK)] = {0, 0, 0, 255};
    _colors[static_cast<int>(Color::RED)] = {255, 0, 0, 255};
    _colors[static_cast<int>(Color::GREEN)] = {0, 255, 0, 255};
    _colors[static_cast<int>(Color::YELLOW)] = {255, 255, 0, 255};
    _colors[static_cast<int>(Color::BLUE)] = {0, 0, 255, 255};
    _colors[static_cast<int>(Color::MAGENTA)] = {255, 0, 255, 255};
    _colors[static_cast<int>(Color::CYAN)] = {0, 255, 255, 255};
    _colors[static_cast<int>(Color::WHITE)] = {255, 255, 255, 255};
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

    updateWindowSize();
    SDL_RenderPresent(_renderer);
    _frameCounter++;
}

void SDL2Graphics::showSplashScreen()
{
    if (!_initialized)
        return;

    clear();
    std::string title = "Arcade";
    drawTextCentered(_height / 2, title, Color::CYAN);
    refresh();
    SDL_Delay(2000);
    clear();
    refresh();
}

void SDL2Graphics::drawText(int x, int y, const std::string& text, Color color)
{
    if (!_initialized || text.empty())
        return;

    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_Surface* surface = TTF_RenderText_Solid(_font, text.c_str(), sdlColor);
    if (!surface)
        return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = surface->w;
    dest.h = surface->h;

    SDL_RenderCopy(_renderer, texture, nullptr, &dest);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void SDL2Graphics::drawTextCentered(int y, const std::string& text, Color color)
{
    if (!_initialized)
        return;

    int textWidth, textHeight;
    TTF_SizeText(_font, text.c_str(), &textWidth, &textHeight);
    int x = (_width - textWidth) / 2;
    drawText(x, y, text, color);
}

void SDL2Graphics::drawBoldText(int x, int y, const std::string& text, Color color)
{
    drawText(x, y, text, color);
}

void SDL2Graphics::drawBox(int x, int y, int width, int height, Color color)
{
    if (!_initialized)
        return;

    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_SetRenderDrawColor(_renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = width;
    rect.h = height;

    SDL_RenderDrawRect(_renderer, &rect);
}

void SDL2Graphics::drawBoxWithTitle(int x, int y, int width, int height, const std::string& title, Color color)
{
    drawBox(x, y, width, height, color);
    int titleX = x + (width - title.length() * 8) / 2;
    drawText(titleX, y - 10, title, color);
}

void SDL2Graphics::drawFilledBox(int x, int y, int width, int height, char fillChar, Color color)
{
    if (!_initialized)
        return;

    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_SetRenderDrawColor(_renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);

    SDL_Rect rect = {x, y, width, height};
    SDL_RenderFillRect(_renderer, &rect);

    if (fillChar != ' ') {
        int charWidth, charHeight;
        TTF_SizeText(_font, &fillChar, &charWidth, &charHeight);
        
        for (int i = 0; i < height; i += charHeight) {
            for (int j = 0; j < width; j += charWidth) {
                drawText(x + j, y + i, std::string(1, fillChar), color);
            }
        }
    }
}

void SDL2Graphics::drawHorizontalLine(int x, int y, int width, Color color)
{
    if (!_initialized)
        return;

    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_SetRenderDrawColor(_renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);
    SDL_RenderDrawLine(_renderer, x, y, x + width, y);
}

void SDL2Graphics::drawVerticalLine(int x, int y, int height, Color color)
{
    if (!_initialized)
        return;

    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_SetRenderDrawColor(_renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);
    SDL_RenderDrawLine(_renderer, x, y, x, y + height);
}

void SDL2Graphics::drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color)
{
    if (!_initialized)
        return;

    int lineHeight = TTF_FontHeight(_font);
    for (size_t i = 0; i < items.size(); ++i) {
        if (static_cast<int>(i) == selectedIndex) {
            drawText(x, y + i * lineHeight, "> " + items[i], Color::YELLOW);
        } else {
            drawText(x, y + i * lineHeight, "  " + items[i], color);
        }
    }
}

void SDL2Graphics::drawProgressBar(int x, int y, int width, int value, int maxValue, Color color)
{
    if (!_initialized)
        return;

    drawBox(x, y, width, 10, color);
    int progress = static_cast<int>((static_cast<float>(value) / maxValue) * (width - 4));
    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_SetRenderDrawColor(_renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);
    SDL_Rect rect = {x + 2, y + 2, progress, 6};
    SDL_RenderFillRect(_renderer, &rect);
}

int SDL2Graphics::mapKeyCode(SDL_Keycode sdlKey)
{
    switch (sdlKey) {
        case SDLK_UP: return KeyCode::UP;
        case SDLK_DOWN: return KeyCode::DOWN;
        case SDLK_LEFT: return KeyCode::LEFT;
        case SDLK_RIGHT: return KeyCode::RIGHT;
        case SDLK_ESCAPE: return KeyCode::ESC;
        case SDLK_RETURN: return KeyCode::ENTER;
        case SDLK_SPACE: return KeyCode::SPACE;
        case SDLK_TAB: return KeyCode::TAB;
        case SDLK_BACKSPACE: return KeyCode::BACKSPACE;
        case SDLK_r: return KeyCode::RESTART;
        case SDLK_q: return KeyCode::QUIT;
        case SDLK_p: return KeyCode::PAUSE;
        case SDLK_7: return KeyCode::NEXT_GAME;
        case SDLK_8: return KeyCode::PREV_GAME;
        case SDLK_9: return KeyCode::NEXT_LIB;
        case SDLK_0: return KeyCode::PREV_LIB;
        default: return sdlKey;
    }
}

std::optional<std::unique_ptr<IEvent>> SDL2Graphics::pollEvent()
{
    if (!_initialized)
        return std::nullopt;

    SDL_Event sdlEvent;
    if (SDL_PollEvent(&sdlEvent)) {
        switch (sdlEvent.type) {
            case SDL_QUIT:
                return IEvent::createKeyEvent(KeyCode::QUIT, true);
            case SDL_KEYDOWN: {
                int keyCode = mapKeyCode(sdlEvent.key.keysym.sym);
                return IEvent::createKeyEvent(keyCode, true);
            }
            case SDL_KEYUP: {
                int keyCode = mapKeyCode(sdlEvent.key.keysym.sym);
                return IEvent::createKeyEvent(keyCode, false);
            }
            case SDL_MOUSEBUTTONDOWN:
                return IEvent::createMouseEvent(
                    sdlEvent.button.x, 
                    sdlEvent.button.y, 
                    sdlEvent.button.button, 
                    EventType::MOUSE_PRESSED
                );
            case SDL_MOUSEBUTTONUP:
                return IEvent::createMouseEvent(
                    sdlEvent.button.x, 
                    sdlEvent.button.y, 
                    sdlEvent.button.button, 
                    EventType::MOUSE_RELEASED
                );
            case SDL_MOUSEMOTION:
                return IEvent::createMouseEvent(
                    sdlEvent.motion.x, 
                    sdlEvent.motion.y, 
                    0,
                    EventType::MOUSE_MOVED
                );
            case SDL_WINDOWEVENT:
                if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED) {
                    updateWindowSize();
                    return IEvent::createWindowEvent(_width, _height);
                }
                break;
            default:
                break;
        }
    }
    return std::nullopt;
}

void SDL2Graphics::renderEntity(const Entity& entity)
{
    if (!_initialized)
        return;

    Color color = Color::WHITE;
    if (entity.colorName == "BLACK") color = Color::BLACK;
    else if (entity.colorName == "RED") color = Color::RED;
    else if (entity.colorName == "GREEN") color = Color::GREEN;
    else if (entity.colorName == "YELLOW") color = Color::YELLOW;
    else if (entity.colorName == "BLUE") color = Color::BLUE;
    else if (entity.colorName == "MAGENTA") color = Color::MAGENTA;
    else if (entity.colorName == "CYAN") color = Color::CYAN;

    SDL_Color sdlColor = _colors[static_cast<int>(color)];
    SDL_SetRenderDrawColor(_renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);

    switch (entity.type) {
        case EntityType::PLAYER:
            {
                SDL_Rect rect = {entity.x, entity.y, entity.width, entity.height};
                SDL_RenderFillRect(_renderer, &rect);
                auto it = entity.properties.find("isHead");
                if (it != entity.properties.end() && std::any_cast<bool>(it->second)) {
                    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
                    SDL_Rect eyeRect = {entity.x + entity.width/4, entity.y + entity.height/4, 
                                        entity.width/8, entity.height/8};
                    SDL_RenderFillRect(_renderer, &eyeRect);
                }
            }
            break;
        case EntityType::COLLECTIBLE:
            {
                int centerX = entity.x + entity.width / 2;
                int centerY = entity.y + entity.height / 2;
                int radius = std::min(entity.width, entity.height) / 2;
                
                for (int i = -radius; i <= radius; i++) {
                    int width = 2 * static_cast<int>(std::sqrt(radius * radius - i * i));
                    SDL_Rect rect = {centerX - width/2, centerY + i, width, 1};
                    SDL_RenderFillRect(_renderer, &rect);
                }
            }
            break;
        case EntityType::OBSTACLE:
            {
                SDL_Rect rect = {entity.x, entity.y, entity.width, entity.height};
                SDL_RenderDrawRect(_renderer, &rect);
            }
            break;
        default:
            if (!entity.symbol.empty()) {
                drawText(entity.x, entity.y, entity.symbol, color);
            } else {
                SDL_Rect rect = {entity.x, entity.y, entity.width, entity.height};
                SDL_RenderFillRect(_renderer, &rect);
            }
            break;
    }
}

void SDL2Graphics::renderGameState(const IGameState& gameState)
{
    if (!_initialized)
        return;

    clear();
    int offsetX = (_width - gameState.getWidth() * 20) / 2;
    int offsetY = (_height - gameState.getHeight() * 20) / 2;
    
    SDL_SetRenderDrawColor(_renderer, 100, 100, 100, 255);
    SDL_Rect borderRect = {offsetX - 10, offsetY - 10, 
                          gameState.getWidth() * 20 + 20, 
                          gameState.getHeight() * 20 + 20};
    SDL_RenderDrawRect(_renderer, &borderRect);
    
    for (const auto& entity : gameState.getEntities()) {
        Entity scaledEntity = entity;
        scaledEntity.x = entity.x * 20 + offsetX;
        scaledEntity.y = entity.y * 20 + offsetY;
        scaledEntity.width = entity.width * 20;
        scaledEntity.height = entity.height * 20;
        
        renderEntity(scaledEntity);
    }
    
    std::string scoreText = "Score: " + std::to_string(gameState.getScore());
    drawText(10, 10, scoreText, Color::YELLOW);
    
    if (gameState.isGameOver()) {
        std::string gameOverText = "Game Over!";
        drawTextCentered(_height - 30, gameOverText, Color::RED);
    }
    
    if (!gameState.getMessage().empty()) {
        drawTextCentered(_height - 50, gameState.getMessage(), Color::WHITE);
    }
    
    refresh();
}

void SDL2Graphics::renderUI(const std::vector<UIElement>& uiElements)
{
    if (!_initialized)
        return;

    for (const auto& element : uiElements) {
        switch (element.type) {
            case UIElementType::TEXT:
                if (element.selected) {
                    drawBoldText(element.x, element.y, element.text, element.color);
                } else {
                    drawText(element.x, element.y, element.text, element.color);
                }
                break;
            case UIElementType::BUTTON:
                {
                    SDL_Color buttonColor = _colors[static_cast<int>(element.color)];
                    SDL_SetRenderDrawColor(_renderer, buttonColor.r, buttonColor.g, buttonColor.b, 100);
                    SDL_Rect btnRect = {element.x, element.y, element.width, element.height};
                    SDL_RenderFillRect(_renderer, &btnRect);
                    
                    SDL_SetRenderDrawColor(_renderer, buttonColor.r, buttonColor.g, buttonColor.b, 255);
                    SDL_RenderDrawRect(_renderer, &btnRect);
                    
                    int textWidth, textHeight;
                    TTF_SizeText(_font, element.text.c_str(), &textWidth, &textHeight);
                    int textX = element.x + (element.width - textWidth) / 2;
                    int textY = element.y + (element.height - textHeight) / 2;
                    
                    if (element.selected) {
                        drawBoldText(textX, textY, element.text, element.color);
                    } else {
                        drawText(textX, textY, element.text, element.color);
                    }
                }
                break;
            case UIElementType::LIST:
                {
                    auto items = std::any_cast<std::vector<std::string>>(
                        element.properties.at("items"));
                    int selectedIndex = std::any_cast<int>(
                        element.properties.at("selectedIndex"));
                    drawList(element.x, element.y, items, selectedIndex, element.color);
                }
                break;
            case UIElementType::PROGRESS_BAR:
                {
                    int value = std::any_cast<int>(element.properties.at("value"));
                    int maxValue = std::any_cast<int>(element.properties.at("maxValue"));
                    drawProgressBar(element.x, element.y, element.width, value, maxValue, element.color);
                }
                break;
            case UIElementType::PANEL:
                {
                    drawBox(element.x, element.y, element.width, element.height, element.color);
                    if (!element.text.empty()) {
                        drawBoxWithTitle(element.x, element.y, element.width, element.height, element.text, element.color);
                    }
                }
                break;
            case UIElementType::INPUT_FIELD:
                {
                    drawBox(element.x, element.y, element.width, element.height, element.color);
                    drawText(element.x + 5, element.y + element.height/2 - 7, element.text, element.color);
                }
                break;
            default:
                break;
        }
    }
}

void SDL2Graphics::getPlayerName(std::string& playerName)
{
    if (!_initialized) return;
    
    // Préparer l'interface pour la saisie du nom
    SDL_StartTextInput();
    
    // Texte initial
    playerName.clear();
    bool done = false;
    bool canceled = false;
    
    // Boucle de saisie
    while (!done && _initialized) {
        // Dessiner la boîte de saisie
        clear();
        
        // Afficher le texte actuel avec un curseur clignotant
        std::string displayText = playerName + (SDL_GetTicks() % 1000 < 500 ? "_" : "");
        
        int boxWidth = 300;
        int boxHeight = 40;
        int boxX = (_width - boxWidth) / 2;
        int boxY = (_height - boxHeight) / 2;
        
        // Dessiner la boîte
        SDL_Rect boxRect = { boxX, boxY, boxWidth, boxHeight };
        SDL_SetRenderDrawColor(_renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(_renderer, &boxRect);
        SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(_renderer, &boxRect);
        
        // Dessiner le texte de saisie
        drawText(boxX + 10, boxY + 10, displayText, Color::WHITE);
        
        // Rafraîchir l'écran
        refresh();
        
        // Gérer les événements
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                canceled = true;
                done = true;
            } 
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN) {
                    done = true;
                } 
                else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    canceled = true;
                    done = true;
                } 
                else if (event.key.keysym.sym == SDLK_BACKSPACE && !playerName.empty()) {
                    playerName.pop_back();
                }
            } 
            else if (event.type == SDL_TEXTINPUT) {
                // Ajouter le texte saisi si pas trop long
                if (playerName.length() < 20) {
                    playerName += event.text.text;
                }
            }
        }
    }
    
    // Désactiver la saisie de texte
    SDL_StopTextInput();
    
    // Effacer si annulé
    if (canceled) {
        playerName.clear();
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