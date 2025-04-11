/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** SFMLGraphics implementation
*/

#include "graphics/SFMLGraphics.hpp"
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <filesystem>
#include <chrono>
#include <thread>
#include <cmath>

namespace arcd {

SFMLGraphics::SFMLGraphics()
    : _initialized(false), _width(800), _height(600), _lastKey(0), _frameCounter(0)
{
}

SFMLGraphics::~SFMLGraphics()
{
    cleanup();
}

bool SFMLGraphics::initialize()
{
    if (_initialized)
        return true;

    _window.create(sf::VideoMode(_width, _height, 32), "Arcade - SFML", sf::Style::Default);
    _window.setFramerateLimit(60);

    if (!_font.loadFromFile("assets/fonts/DejaVuSansMono.ttf")) {
        if (!_font.loadFromFile("/usr/share/fonts/TTF/DejaVuSansMono.ttf") && 
            !_font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf")) {
            std::cerr << "Failed to load font" << std::endl;
            _window.close();
            return false;
        }
    }

    initColors();

    // Clear storage containers
    _textElements.clear();
    _rectElements.clear();

    // Show splash screen
    showSplashScreen();

    _initialized = true;
    return true;
}

void SFMLGraphics::cleanup()
{
    if (_initialized) {
        _textElements.clear();
        _rectElements.clear();
        _window.close();
        _initialized = false;
    }
}

void SFMLGraphics::clear()
{
    if (!_initialized)
        return;

    _window.clear(sf::Color::Black);
    _textElements.clear();
    _rectElements.clear();
}

void SFMLGraphics::redrawElements()
{
    for (const auto& rectElem : _rectElements) {
        _window.draw(rectElem.rect);
    }

    for (const auto& textElem : _textElements) {
        _window.draw(textElem.text);
    }
}

void SFMLGraphics::refresh()
{
    if (!_initialized) {
        return;
    }

    redrawElements();
    _window.display();
    _frameCounter++;
    updateWindowSize();
}

void SFMLGraphics::initColors()
{
    _colors[static_cast<int>(Color::DEFAULT)] = sf::Color::White;
    _colors[static_cast<int>(Color::BLACK)] = sf::Color::Black;
    _colors[static_cast<int>(Color::RED)] = sf::Color::Red;
    _colors[static_cast<int>(Color::GREEN)] = sf::Color::Green;
    _colors[static_cast<int>(Color::YELLOW)] = sf::Color::Yellow;
    _colors[static_cast<int>(Color::BLUE)] = sf::Color::Blue;
    _colors[static_cast<int>(Color::MAGENTA)] = sf::Color::Magenta;
    _colors[static_cast<int>(Color::CYAN)] = sf::Color::Cyan;
    _colors[static_cast<int>(Color::WHITE)] = sf::Color::White;
}

void SFMLGraphics::drawText(int x, int y, const std::string& text, Color color)
{
    if (!_initialized || text.empty()) return;

    sf::Text sfText;
    sfText.setFont(_font);
    sfText.setString(text);
    sfText.setCharacterSize(16);
    sfText.setFillColor(_colors[static_cast<int>(color)]);
    sfText.setPosition(static_cast<float>(x), static_cast<float>(y));

    TextDrawElement elem;
    elem.text = sfText;
    elem.position = sf::Vector2f(x, y);
    _textElements.push_back(elem);

    _window.draw(sfText);
}

void SFMLGraphics::drawBox(int x, int y, int width, int height, Color color)
{
    if (!_initialized)
        return;

    sf::RectangleShape rect(sf::Vector2f(width * 10.0f, height * 20.0f));
    rect.setPosition(sf::Vector2f(x * 10.0f, y * 20.0f));
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(_colors[static_cast<int>(color)]);
    rect.setOutlineThickness(1.0f);

    RectDrawElement elem{rect};
    _rectElements.push_back(elem);

    _window.draw(rect);
}

void SFMLGraphics::drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color)
{
    if (!_initialized)
        return;

    for (size_t i = 0; i < items.size(); ++i) {
        Color itemColor = (static_cast<int>(i) == selectedIndex) ? Color::YELLOW : color;
        std::string displayText;

        // Draw selection indicator
        if (static_cast<int>(i) == selectedIndex) {
            displayText = "> " + items[i];
        } else {
            displayText = "  " + items[i];
        }

        // Draw item text
        sf::Text text;
        text.setFont(_font);
        text.setString(displayText);
        text.setCharacterSize(16);
        text.setFillColor(_colors[static_cast<int>(itemColor)]);
        text.setPosition(sf::Vector2f(x * 10.0f, (y + i) * 20.0f));

        TextDrawElement textElem{text, sf::Vector2f(x * 10.0f, (y + i) * 20.0f)};
        _textElements.push_back(textElem);

        _window.draw(text);
    }
}

int SFMLGraphics::getKey()
{
    if (!_initialized)
        return 0;

    // Si une touche a été traitée dans refresh(), la retourner
    if (_lastKey != 0) {
        int key = _lastKey;
        _lastKey = 0;
        return key;
    }

    // Sinon, vérifier les événements pour une nouvelle touche
    sf::Event event;
    while (_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            _window.close();
            _initialized = false;
            return KeyCode::ESC;
        }
        if (event.type == sf::Event::KeyPressed) {
            return handleKeyPress(event.key.code);
        }
    }

    return 0;
}

void SFMLGraphics::getPlayerName(std::string& playerName)
{
    if (!_initialized)
        return;

    playerName.clear();
    bool done = false;

    while (!done && _window.isOpen()) {
        clear();
        drawText(10, 10, "Enter your name: " + playerName + "_", Color::WHITE);
        _window.display();

        sf::Event event;
        while (_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                done = true;
                _window.close();
                _initialized = false;
                break;
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Key::Enter && !playerName.empty()) {
                    done = true;
                } else if (event.key.code == sf::Keyboard::Key::Backspace && !playerName.empty()) {
                    playerName.pop_back();
                } else if (event.key.code == sf::Keyboard::Key::Escape) {
                    playerName.clear();
                    done = true;
                } else if (playerName.length() < 20) {
                    // Handle text input
                    if (event.key.code >= sf::Keyboard::Key::A && event.key.code <= sf::Keyboard::Key::Z) {
                        char c = 'a' + (static_cast<int>(event.key.code) - static_cast<int>(sf::Keyboard::Key::A));
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || 
                            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)) {
                            c = std::toupper(c);
                        }
                        playerName += c;
                    } else if (event.key.code >= sf::Keyboard::Key::Num0 && event.key.code <= sf::Keyboard::Key::Num9) {
                        char c = '0' + (static_cast<int>(event.key.code) - static_cast<int>(sf::Keyboard::Key::Num0));
                        playerName += c;
                    }
                }
            }
        }
    }
}

std::string SFMLGraphics::getName() const
{
    return "SFML";
}

int SFMLGraphics::waitForKey(int timeoutMs)
{
    if (!_initialized) return 0;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    sf::Event event;
    
    while (true) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
        
        if (elapsedTime >= timeoutMs) {
            return 0;
        }
        
        if (_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return KeyCode::ESC;
            }
            
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Key::Up: return KeyCode::UP;
                    case sf::Keyboard::Key::Down: return KeyCode::DOWN;
                    case sf::Keyboard::Key::Right: return KeyCode::RIGHT;
                    case sf::Keyboard::Key::Left: return KeyCode::LEFT;
                    case sf::Keyboard::Key::Enter: return KeyCode::ENTER;
                    case sf::Keyboard::Key::Escape: return KeyCode::ESC;
                    case sf::Keyboard::Key::Backspace: return KeyCode::BACKSPACE;
                    case sf::Keyboard::Key::Num9: return KeyCode::NEXT_LIB;
                    case sf::Keyboard::Key::Num7: return KeyCode::NEXT_GAME;
                    default: return static_cast<int>(event.key.code);
                }
            }
        }
        
        // Small sleep to avoid consuming too much CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    
    return 0;
}

void SFMLGraphics::waitForAnyKey()
{
    if (!_initialized)
        return;
    while (_window.isOpen()) {
        if (waitForKey(100) != 0) {
            break;
        }
    }
}

void SFMLGraphics::drawTextCentered(int y, const std::string& text, Color color)
{
    if (!_initialized || text.empty()) return;
    
    sf::Text sfText;
    sfText.setFont(_font);
    sfText.setString(text);
    sfText.setCharacterSize(16);
    sfText.setFillColor(_colors[static_cast<int>(color)]);
    
    // Calculate center position
    sf::FloatRect textRect = sfText.getLocalBounds();
    int x = static_cast<int>((_width - textRect.width) / 2);
    
    sfText.setPosition(static_cast<float>(x), static_cast<float>(y));
    
    // Store for later drawing
    TextDrawElement element;
    element.text = sfText;
    element.position = sf::Vector2f(x, y);
    _textElements.push_back(element);
}

void SFMLGraphics::drawBoxWithTitle(int x, int y, int width, int height, const std::string& title, Color color)
{
    drawBox(x, y, width, height, color);
    
    // Draw title at the top center of the box
    sf::Text sfText;
    sfText.setFont(_font);
    sfText.setString(title);
    sfText.setCharacterSize(16);
    sfText.setFillColor(_colors[static_cast<int>(color)]);
    
    // Calculate center position
    sf::FloatRect textRect = sfText.getLocalBounds();
    int titleX = x + (width - static_cast<int>(textRect.width)) / 2;
    
    drawText(titleX, y - 20, title, color);
}

void SFMLGraphics::drawFilledBox(int x, int y, int width, int height, char fillChar, Color color)
{
    if (!_initialized)
        return;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            sf::Text sfText;
            sfText.setFont(_font);
            sfText.setString(std::string(1, fillChar));
            sfText.setCharacterSize(16);
            sfText.setFillColor(_colors[static_cast<int>(color)]);
            sfText.setPosition(sf::Vector2f((x + j) * 10.0f, (y + i) * 20.0f));

            TextDrawElement elem{sfText, sf::Vector2f((x + j) * 10.0f, (y + i) * 20.0f)};
            _textElements.push_back(elem);

            _window.draw(sfText);
        }
    }
}

void SFMLGraphics::drawHorizontalLine(int x, int y, int width, Color color)
{
    if (!_initialized)
        return;

    sf::RectangleShape line(sf::Vector2f(width * 10.0f, 1.0f));
    line.setPosition(sf::Vector2f(x * 10.0f, y * 20.0f));
    line.setFillColor(_colors[static_cast<int>(color)]);

    RectDrawElement elem{line};
    _rectElements.push_back(elem);

    _window.draw(line);
}

void SFMLGraphics::drawVerticalLine(int x, int y, int height, Color color)
{
    if (!_initialized)
        return;

    sf::RectangleShape line(sf::Vector2f(1.0f, height * 20.0f));
    line.setPosition(sf::Vector2f(x * 10.0f, y * 20.0f));
    line.setFillColor(_colors[static_cast<int>(color)]);

    RectDrawElement elem{line};
    _rectElements.push_back(elem);

    _window.draw(line);
}

void SFMLGraphics::drawProgressBar(int x, int y, int width, int value, int maxValue, Color color)
{
    if (!_initialized)
        return;

    sf::RectangleShape background(sf::Vector2f(width * 10.0f, 10.0f));
    background.setPosition(sf::Vector2f(x * 10.0f, y * 20.0f));
    background.setFillColor(sf::Color(50, 50, 50));

    RectDrawElement bgElem{background};
    _rectElements.push_back(bgElem);

    // Draw progress
    int progress = static_cast<int>((static_cast<float>(value) / maxValue) * (width - 2));
    sf::RectangleShape progressBar(sf::Vector2f(progress * 10.0f, 10.0f));
    progressBar.setPosition(sf::Vector2f((x + 1) * 10.0f, y * 20.0f));
    progressBar.setFillColor(_colors[static_cast<int>(color)]);

    RectDrawElement progElem{progressBar};
    _rectElements.push_back(progElem);

    _window.draw(background);
    _window.draw(progressBar);
}

void SFMLGraphics::drawBoldText(int x, int y, const std::string& text, Color color)
{
    if (!_initialized || text.empty()) return;
    
    sf::Text sfText;
    sfText.setFont(_font);
    sfText.setString(text);
    sfText.setCharacterSize(16);
    sfText.setStyle(sf::Text::Bold);
    sfText.setFillColor(_colors[static_cast<int>(color)]);
    sfText.setPosition(static_cast<float>(x), static_cast<float>(y));
    
    // Store for later drawing
    TextDrawElement element;
    element.text = sfText;
    element.position = sf::Vector2f(x, y);
    _textElements.push_back(element);
}

int SFMLGraphics::handleKeyPress(sf::Keyboard::Key key)
{
    switch (key) {
        case sf::Keyboard::Up:
            return KeyCode::UP;
        case sf::Keyboard::Down:
            return KeyCode::DOWN;
        case sf::Keyboard::Left:
            return KeyCode::LEFT;
        case sf::Keyboard::Right:
            return KeyCode::RIGHT;
        case sf::Keyboard::Return:
            return KeyCode::ENTER;
        case sf::Keyboard::Escape:
            return KeyCode::ESC;
        case sf::Keyboard::BackSpace:
            return KeyCode::BACKSPACE;
        case sf::Keyboard::Space:
            return KeyCode::SPACE;
        case sf::Keyboard::Num7:
            return KeyCode::NEXT_GAME;
        case sf::Keyboard::Num8:
            return KeyCode::PREV_GAME;
        case sf::Keyboard::Num9:
            return KeyCode::NEXT_LIB;
        case sf::Keyboard::Num0:
            return KeyCode::PREV_LIB;
        case sf::Keyboard::R:
            return KeyCode::RESTART;
        case sf::Keyboard::P:
            return KeyCode::PAUSE;
        case sf::Keyboard::Q:
            return KeyCode::QUIT;
        default:
            return static_cast<int>(key);
    }
}

int SFMLGraphics::mapKeyCode(sf::Keyboard::Key sfmlKey)
{
    switch (sfmlKey) {
        case sf::Keyboard::Key::Up: return KeyCode::UP;
        case sf::Keyboard::Key::Down: return KeyCode::DOWN;
        case sf::Keyboard::Key::Left: return KeyCode::LEFT;
        case sf::Keyboard::Key::Right: return KeyCode::RIGHT;
        case sf::Keyboard::Key::Escape: return KeyCode::ESC;
        case sf::Keyboard::Key::BackSpace: return KeyCode::BACKSPACE;
        case sf::Keyboard::Key::Return: return KeyCode::ENTER;
        case sf::Keyboard::Key::Space: return KeyCode::SPACE;
        case sf::Keyboard::Key::Num9: return KeyCode::NEXT_LIB;
        case sf::Keyboard::Key::Num7: return KeyCode::NEXT_GAME;
        case sf::Keyboard::Key::Num8: return KeyCode::PREV_GAME;
        case sf::Keyboard::Key::Num0: return KeyCode::PREV_LIB;
        case sf::Keyboard::Key::R: return KeyCode::RESTART;
        case sf::Keyboard::Key::P: return KeyCode::PAUSE;
        case sf::Keyboard::Key::Q: return KeyCode::QUIT;
        default: return static_cast<int>(sfmlKey);
    }
}

std::optional<std::unique_ptr<IEvent>> SFMLGraphics::pollEvent()
{
    if (!_initialized)
        return std::nullopt;

    sf::Event event;
    while (_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            _window.close();
            _initialized = false;
            std::cout << "SFML: Window closed, sending ESC key event" << std::endl;
            return std::make_optional(IEvent::createKeyEvent(KeyCode::ESC, true));
        }
        else if (event.type == sf::Event::KeyPressed) {
            int keyCode = handleKeyPress(event.key.code);
            std::cout << "SFML: Key pressed: " << keyCode << std::endl;
            return std::make_optional(IEvent::createKeyEvent(keyCode, true));
        }
    }
    
    return std::nullopt;
}

void SFMLGraphics::renderEntity(const Entity& entity)
{
    if (!_initialized) return;
    
    // Get entity display properties
    int x = entity.x;
    int y = entity.y;
    char displayChar = entity.symbol.empty() ? '?' : entity.symbol[0];
    
    // Convert colorName to Color enum
    Color color = Color::WHITE;
    if (entity.colorName == "BLACK") color = Color::BLACK;
    else if (entity.colorName == "RED") color = Color::RED;
    else if (entity.colorName == "GREEN") color = Color::GREEN;
    else if (entity.colorName == "YELLOW") color = Color::YELLOW;
    else if (entity.colorName == "BLUE") color = Color::BLUE;
    else if (entity.colorName == "MAGENTA") color = Color::MAGENTA;
    else if (entity.colorName == "CYAN") color = Color::CYAN;
    
    // Draw entity as a filled rectangle with the color and symbol
    drawFilledBox(x, y, entity.width, entity.height, displayChar, color);
}

void SFMLGraphics::renderGameState(const IGameState& gameState)
{
    if (!_initialized) return;
    
    clear();
    
    // Draw border around game area
    int gridWidth = gameState.getWidth();
    int gridHeight = gameState.getHeight();
    int cellSize = 20;  // Default cell size
    
    int gameAreaWidth = gridWidth * cellSize;
    int gameAreaHeight = gridHeight * cellSize;
    
    // Center the game area on screen
    int startX = (_width - gameAreaWidth) / 2;
    int startY = (_height - gameAreaHeight) / 2;
    
    // Draw outer border
    drawBox(startX - 5, startY - 5, gameAreaWidth + 10, gameAreaHeight + 10, Color::WHITE);
    
    // Draw grid lines (optional)
    for (int x = 0; x <= gridWidth; ++x) {
        drawVerticalLine(startX + x * cellSize, startY, gameAreaHeight, Color::BLUE);
    }
    
    for (int y = 0; y <= gridHeight; ++y) {
        drawHorizontalLine(startX, startY + y * cellSize, gameAreaWidth, Color::BLUE);
    }
    
    // Draw all entities
    for (const auto& entity : gameState.getEntities()) {
        // Adjust entity position by the start offset
        Entity adjustedEntity = entity;
        adjustedEntity.x = startX + entity.x * cellSize;
        adjustedEntity.y = startY + entity.y * cellSize;
        renderEntity(adjustedEntity);
    }
    
    // Display game information - use the IGameState interface methods
    std::string scoreText = "Score: " + std::to_string(gameState.getScore());
    drawText(10, 10, scoreText, Color::YELLOW);
    
    // Draw game message if available
    if (!gameState.getMessage().empty()) {
        drawTextCentered(_height / 2 + 30, gameState.getMessage(), Color::WHITE);
    }
    
    // Draw game over message if applicable
    if (gameState.isGameOver()) {
        drawTextCentered(_height / 2 - 20, "GAME OVER", Color::RED);
    }
    
    refresh();
}

void SFMLGraphics::renderUI(const std::vector<UIElement>& uiElements)
{
    if (!_initialized) return;
    
    clear();  // Assurons-nous que l'écran est vide

    // Traiter chaque élément UI
    for (const auto& element : uiElements) {
        switch (element.type) {
            case UIElementType::TEXT:
                {
                    Color textColor = element.selected ? Color::YELLOW : element.color;
                    // Indicator for selected items
                    if (element.selected) {
                        std::string indicator = "> ";
                        drawText(element.x - 20, element.y, indicator, Color::YELLOW);
                    }
                    drawText(element.x, element.y, element.text, textColor);
                }
                break;
                
            case UIElementType::BUTTON:
                {
                    // Dessiner le fond du bouton
                    sf::RectangleShape buttonBg(sf::Vector2f(element.width, element.height));
                    buttonBg.setPosition(element.x, element.y);
                    buttonBg.setFillColor(sf::Color(50, 50, 50, 200));
                    buttonBg.setOutlineThickness(1.0f);
                    buttonBg.setOutlineColor(_colors[static_cast<int>(element.color)]);
                    _window.draw(buttonBg);
                    
                    // Dessiner le texte centré
                    sf::Text buttonText;
                    buttonText.setFont(_font);
                    buttonText.setString(element.text);
                    buttonText.setCharacterSize(16);
                    buttonText.setFillColor(_colors[static_cast<int>(element.color)]);
                    
                    // Calculer le centrage
                    sf::FloatRect textRect = buttonText.getLocalBounds();
                    float x = element.x + (element.width - textRect.width) / 2;
                    float y = element.y + (element.height - textRect.height) / 2;
                    buttonText.setPosition(x, y);
                    
                    _window.draw(buttonText);
                }
                break;
                
            case UIElementType::LIST:
                if (element.properties.count("items") && element.properties.count("selectedIndex")) {
                    try {
                        const auto& items = std::any_cast<std::vector<std::string>>(element.properties.at("items"));
                        int selectedIndex = std::any_cast<int>(element.properties.at("selectedIndex"));
                        drawList(element.x, element.y, items, selectedIndex, element.color);
                    } catch (const std::bad_any_cast&) {
                        // Ignorer les erreurs de cast
                    }
                }
                break;
                
            case UIElementType::PANEL:
                drawBox(element.x, element.y, element.width, element.height, element.color);
                break;
                
            case UIElementType::PROGRESS_BAR:
                if (element.properties.count("value") && element.properties.count("maxValue")) {
                    try {
                        int value = std::any_cast<int>(element.properties.at("value"));
                        int maxValue = std::any_cast<int>(element.properties.at("maxValue"));
                        drawProgressBar(element.x, element.y, element.width, value, maxValue, element.color);
                    } catch (const std::bad_any_cast&) {
                        // Ignorer les erreurs de cast
                    }
                }
                break;
                
            default:
                // Ignorer les types non implémentés
                break;
        }
    }
    
    refresh();  // Actualiser l'affichage
}

// Helper methods implementation
void SFMLGraphics::updateWindowSize()
{
    if (!_initialized)
        return;
    _width = _window.getSize().x;
    _height = _window.getSize().y;
}

void SFMLGraphics::showSplashScreen()
{
    if (!_initialized)
        return;

    std::cout << "Showing splash screen" << std::endl;

    clear();

    // Draw simple test elements to ensure rendering works
    sf::RectangleShape background(sf::Vector2f(_width, _height));
    background.setFillColor(sf::Color(30, 30, 30));

    RectDrawElement bgElem{background};
    _rectElements.push_back(bgElem);

    // Draw title box
    int titleWidth = 40;
    int titleHeight = 3;
    int startX = (_width / 10 - titleWidth) / 2;
    int startY = (_height / 20 - titleHeight) / 2;

    sf::RectangleShape box(sf::Vector2f(titleWidth * 10.0f, titleHeight * 20.0f));
    box.setPosition(sf::Vector2f(startX * 10.0f, startY * 20.0f));
    box.setFillColor(sf::Color::Transparent);
    box.setOutlineColor(sf::Color::Cyan);
    box.setOutlineThickness(2.0f);

    RectDrawElement boxElem{box};
    _rectElements.push_back(boxElem);

    // Draw title text
    sf::Text titleText;
    titleText.setFont(_font);
    titleText.setString("ARCADE");
    titleText.setCharacterSize(32);
    titleText.setFillColor(sf::Color::Cyan);
    titleText.setStyle(sf::Text::Bold);
    
    // Centrer le titre
    sf::FloatRect titleRect = titleText.getLocalBounds();
    float centerX = startX * 10.0f + (titleWidth * 10.0f - titleRect.width) / 2.0f;
    float centerY = startY * 20.0f + (titleHeight * 20.0f - titleRect.height) / 2.0f - 10.0f;
    titleText.setPosition(sf::Vector2f(centerX, centerY));

    TextDrawElement titleElem{titleText, sf::Vector2f(centerX, centerY)};
    _textElements.push_back(titleElem);

    // Additional diagnostic text
    sf::Text debugText;
    debugText.setFont(_font);
    debugText.setString("Press any key or wait 2 seconds");
    debugText.setCharacterSize(16);
    debugText.setFillColor(sf::Color::White);
    debugText.setPosition(sf::Vector2f(50.0f, _height - 50.0f));

    TextDrawElement debugElem{debugText, sf::Vector2f(50.0f, _height - 50.0f)};
    _textElements.push_back(debugElem);

    // Draw all elements
    _window.clear(sf::Color::Black);
    redrawElements();
    _window.display();

    std::cout << "Splash screen rendered" << std::endl;

    // Wait for 2 seconds
    sf::Clock clock;
    while (clock.getElapsedTime().asSeconds() < 2.0f && _window.isOpen()) {
        sf::Event event;
        while (_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                _window.close();
                _initialized = false;
                return;
            } else if (event.type == sf::Event::KeyPressed) {
                break;
            }
        }
    }

    std::cout << "Splash screen complete" << std::endl;
    clear();
    _window.display();
}

extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary()
    {
        return std::make_unique<arcd::SFMLGraphics>();
    }
    
    void destroyGraphicsLibrary([[maybe_unused]] arcd::IGraphicsLibrary* graphicsLib)
    {
        // With unique_ptr, this function is not needed anymore
        // but we keep it for compatibility
    }
}

}  // namespace arcd 