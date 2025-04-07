/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** SFMLGraphics implementation
*/

#include "../../include/graphics/SFMLGraphics.hpp"
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <filesystem>

namespace arcd {

SFMLGraphics::SFMLGraphics()
    : _initialized(false), _width(800), _height(600), _lastKey(0), _frameCounter(0), _selectedMenuItem(0)
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
    
    // Clear storage containers
    _textElements.clear();
    _rectElements.clear();
    
    // Draw welcome screen to test rendering
    drawTestScreen();
    
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
    
    // Ne stockons plus d'éléments, simplement efface l'écran
    _window.clear(sf::Color(20, 20, 50));
}

void SFMLGraphics::redrawElements()
{
    // Redraw all stored elements
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
    
    // Vérifier les événements
    std::optional<sf::Event> eventOpt;
    
    while ((eventOpt = _window.pollEvent())) {
        const sf::Event& event = *eventOpt;
        if (event.is<sf::Event::Closed>()) {
            _window.close();
            _initialized = false;
            return;
        }
        
        // Traiter les touches pour naviguer dans le menu
        if (event.is<sf::Event::KeyPressed>()) {
            const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();
            if (keyPressed) {
                _lastKey = handleKeyPress(keyPressed->code);
            }
        }
    }
    
    // Vérifier si la fenêtre est toujours ouverte
    if (!_window.isOpen()) {
        _initialized = false;
        return;
    }
    
    // Redessiner à chaque frame pour une meilleure fluidité
    drawTestScreen();
    
    _frameCounter++;
}

void SFMLGraphics::initColors()
{
    _colors[static_cast<int>(Color::DEFAULT)] = sf::Color::White;
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
    if (!_initialized)
        return;

    sf::Text sfText(_font, text, 16);
    sfText.setFillColor(_colors[static_cast<int>(color)]);
    sfText.setPosition(sf::Vector2f(x * 10.0f, y * 20.0f));

    TextDrawElement elem{sfText, sf::Vector2f(x * 10.0f, y * 20.0f)};
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
        Color itemColor = (i == static_cast<size_t>(selectedIndex)) ? Color::YELLOW : color;
        
        // Draw selection indicator
        if (i == static_cast<size_t>(selectedIndex)) {
            sf::Text arrow(_font, ">", 16);
            arrow.setFillColor(_colors[static_cast<int>(itemColor)]);
            arrow.setPosition(sf::Vector2f(x * 10.0f, (y + i) * 20.0f));
            
            TextDrawElement arrowElem{arrow, sf::Vector2f(x * 10.0f, (y + i) * 20.0f)};
            _textElements.push_back(arrowElem);
            
            _window.draw(arrow);
        }
        
        // Draw item text
        sf::Text text(_font, items[i], 16);
        text.setFillColor(_colors[static_cast<int>(itemColor)]);
        text.setPosition(sf::Vector2f((x + 2) * 10.0f, (y + i) * 20.0f));
        
        TextDrawElement textElem{text, sf::Vector2f((x + 2) * 10.0f, (y + i) * 20.0f)};
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
    std::optional<sf::Event> eventOpt = _window.pollEvent();
    if (!eventOpt)
        return 0;
    
    const sf::Event& event = *eventOpt;
    
    if (event.is<sf::Event::Closed>()) {
        _window.close();
        _initialized = false;
        return IGraphicsLibrary::KEY_ESC_CODE;
    }
    
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        return handleKeyPress(keyPressed->code);
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
        
        // Draw immediately
        _window.clear(sf::Color::Black);
        redrawElements();
        _window.display();

        std::optional<sf::Event> eventOpt = _window.pollEvent();
        if (!eventOpt)
            continue;
            
        const sf::Event& event = *eventOpt;
        
        if (event.is<sf::Event::Closed>()) {
            done = true;
            _window.close();
            _initialized = false;
            break;
        }
        
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Enter && !playerName.empty()) {
                done = true;
            } else if (keyPressed->code == sf::Keyboard::Key::Backspace && !playerName.empty()) {
                playerName.pop_back();
            } else if (keyPressed->code == sf::Keyboard::Key::Escape) {
                playerName.clear();
                done = true;
            } else if (playerName.length() < 20) {
                // Handle text input
                if (keyPressed->code >= sf::Keyboard::Key::A && keyPressed->code <= sf::Keyboard::Key::Z) {
                    char c = 'a' + (static_cast<int>(keyPressed->code) - static_cast<int>(sf::Keyboard::Key::A));
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || 
                        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)) {
                        c = std::toupper(c);
                    }
                    playerName += c;
                } else if (keyPressed->code >= sf::Keyboard::Key::Num0 && keyPressed->code <= sf::Keyboard::Key::Num9) {
                    char c = '0' + (static_cast<int>(keyPressed->code) - static_cast<int>(sf::Keyboard::Key::Num0));
                    playerName += c;
                }
            }
        }
    }
}

std::string SFMLGraphics::getName() const
{
    return "SFML";
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
    sf::Text titleText(_font, "ARCADE", 32);
    titleText.setFillColor(sf::Color::Cyan);
    sf::FloatRect textRect = titleText.getLocalBounds();
    float centerX = startX * 10.0f + (titleWidth * 10.0f - textRect.size.x) / 2.0f;
    float centerY = startY * 20.0f + (titleHeight * 20.0f - textRect.size.y) / 2.0f - 10.0f;
    titleText.setPosition(sf::Vector2f(centerX, centerY));
    
    TextDrawElement titleElem{titleText, sf::Vector2f(centerX, centerY)};
    _textElements.push_back(titleElem);
    
    // Additional diagnostic text
    sf::Text debugText(_font, "Press any key or wait 2 seconds", 16);
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
        std::optional<sf::Event> eventOpt = _window.pollEvent();
        if (eventOpt) {
            if (eventOpt->is<sf::Event::Closed>()) {
                _window.close();
                _initialized = false;
                return;
            } else if (eventOpt->is<sf::Event::KeyPressed>()) {
                break;
            }
        }
    }
    
    std::cout << "Splash screen complete" << std::endl;
    clear();
    _window.display();
}

int SFMLGraphics::waitForKey(int timeoutMs)
{
    if (!_initialized)
        return 0;
    
    sf::Clock clock;
    while (clock.getElapsedTime().asMilliseconds() < timeoutMs && _window.isOpen()) {
        std::optional<sf::Event> eventOpt = _window.pollEvent();
        if (!eventOpt)
            continue;
            
        const sf::Event& event = *eventOpt;
        
        if (event.is<sf::Event::Closed>()) {
            _window.close();
            _initialized = false;
            return IGraphicsLibrary::KEY_ESC_CODE;
        }
        
        if (event.is<sf::Event::KeyPressed>()) {
            if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
                switch (keyPressed->code) {
                    case sf::Keyboard::Key::Up: return IGraphicsLibrary::KEY_UP_CODE;
                    case sf::Keyboard::Key::Down: return IGraphicsLibrary::KEY_DOWN_CODE;
                    case sf::Keyboard::Key::Right: return IGraphicsLibrary::KEY_RIGHT_CODE;
                    case sf::Keyboard::Key::Left: return IGraphicsLibrary::KEY_LEFT_CODE;
                    case sf::Keyboard::Key::Enter: return IGraphicsLibrary::KEY_ENTER_CODE;
                    case sf::Keyboard::Key::Escape: return IGraphicsLibrary::KEY_ESC_CODE;
                    case sf::Keyboard::Key::Backspace: return IGraphicsLibrary::KEY_BACKSPACE_CODE;
                    case sf::Keyboard::Key::Num9: return IGraphicsLibrary::KEY_NEXT_LIB_CODE;
                    case sf::Keyboard::Key::Num7: return IGraphicsLibrary::KEY_NEXT_GAME_CODE;
                    default: return static_cast<int>(keyPressed->code);
                }
            }
        }
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
    if (!_initialized)
        return;
    
    sf::Text sfText(_font, text, 16);
    sfText.setFillColor(_colors[static_cast<int>(color)]);
    
    // Center the text
    sf::FloatRect textRect = sfText.getLocalBounds();
    float x = (_width / 10.0f - textRect.size.x / 10.0f) / 2.0f;
    
    sfText.setPosition(sf::Vector2f(x * 10.0f, y * 20.0f));
    
    TextDrawElement elem{sfText, sf::Vector2f(x * 10.0f, y * 20.0f)};
    _textElements.push_back(elem);
    
    _window.draw(sfText);
}

void SFMLGraphics::drawBoxWithTitle(int x, int y, int width, int height, const std::string& title, Color color)
{
    if (!_initialized)
        return;
    
    drawBox(x, y, width, height, color);
    
    // Draw title
    sf::Text sfText(_font, title, 16);
    sfText.setFillColor(_colors[static_cast<int>(color)]);
    
    // Center the title
    sf::FloatRect textRect = sfText.getLocalBounds();
    float titleX = (x + width / 2.0f - textRect.size.x / 20.0f) * 10.0f;
    
    sfText.setPosition(sf::Vector2f(titleX, y * 20.0f));
    
    TextDrawElement elem{sfText, sf::Vector2f(titleX, y * 20.0f)};
    _textElements.push_back(elem);
    
    _window.draw(sfText);
}

void SFMLGraphics::drawFilledBox(int x, int y, int width, int height, char fillChar, Color color)
{
    if (!_initialized)
        return;
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            sf::Text sfText(_font, std::string(1, fillChar), 16);
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
    
    // Draw background
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
    if (!_initialized)
        return;
    
    sf::Text sfText(_font, text, 16);
    sfText.setFillColor(_colors[static_cast<int>(color)]);
    sfText.setStyle(sf::Text::Bold);
    sfText.setPosition(sf::Vector2f(x * 10.0f, y * 20.0f));
    
    TextDrawElement elem{sfText, sf::Vector2f(x * 10.0f, y * 20.0f)};
    _textElements.push_back(elem);
    
    _window.draw(sfText);
}

void SFMLGraphics::drawTestScreen()
{
    // Effacer les collections existantes pour partir d'une base propre
    _textElements.clear();
    _rectElements.clear();
    
    // Effacer la fenêtre avec une couleur de fond
    _window.clear(sf::Color(20, 20, 50));
    
    // Dessiner le fond
    sf::RectangleShape background(sf::Vector2f(_width, _height));
    background.setFillColor(sf::Color(30, 30, 50));
    background.setPosition(sf::Vector2f(0.0f, 0.0f));
    _window.draw(background);
    
    // Dessiner le cadre principal
    sf::RectangleShape frame(sf::Vector2f(_width - 40.0f, _height - 40.0f));
    frame.setPosition(sf::Vector2f(20.0f, 20.0f));
    frame.setFillColor(sf::Color::Transparent);
    frame.setOutlineColor(sf::Color::Cyan);
    frame.setOutlineThickness(2.0f);
    _window.draw(frame);
    
    // Dessiner le titre de l'application
    sf::Text titleText(_font, "ARCADE", 32);
    titleText.setFillColor(sf::Color::Cyan);
    titleText.setStyle(sf::Text::Bold);
    
    // Centrer le titre
    sf::FloatRect titleRect = titleText.getLocalBounds();
    float centerX = (_width - titleRect.size.x) / 2.0f;
    titleText.setPosition(sf::Vector2f(centerX, 40.0f));
    _window.draw(titleText);
    
    // Dessiner le sous-titre
    sf::Text subtitleText(_font, "SFML Graphics Library", 20);
    subtitleText.setFillColor(sf::Color::White);
    
    // Centrer le sous-titre
    sf::FloatRect subtitleRect = subtitleText.getLocalBounds();
    centerX = (_width - subtitleRect.size.x) / 2.0f;
    subtitleText.setPosition(sf::Vector2f(centerX, 90.0f));
    _window.draw(subtitleText);
    
    // Dessiner les options du menu
    std::vector<std::string> menuItems = {
        "Play Game",
        "Select Game",
        "Select Graphics",
        "Quit"
    };
    
    // Mise à jour directe lors de la réception d'un événement clavier dans refresh()
    // La logique est maintenant dans handleKeyPress
    
    float menuY = 160.0f;
    for (size_t i = 0; i < menuItems.size(); i++) {
        sf::Text menuText(_font, menuItems[i], 20);
        menuText.setFillColor(static_cast<int>(i) == _selectedMenuItem ? sf::Color::Yellow : sf::Color::White);
        
        // Centrer l'élément du menu
        sf::FloatRect menuRect = menuText.getLocalBounds();
        centerX = (_width - menuRect.size.x) / 2.0f;
        menuText.setPosition(sf::Vector2f(centerX, menuY + i * 40.0f));
        _window.draw(menuText);
        
        // Ajouter un indicateur de sélection pour l'élément sélectionné
        if (static_cast<int>(i) == _selectedMenuItem) {
            sf::Text selectorText(_font, "> ", 20);
            selectorText.setFillColor(sf::Color::Yellow);
            selectorText.setPosition(sf::Vector2f(centerX - 30.0f, menuY + i * 40.0f));
            _window.draw(selectorText);
        }
    }
    
    // Dessiner les instructions en bas
    sf::Text instructionsText(_font, "Use arrow keys to navigate, Enter to select", 16);
    instructionsText.setFillColor(sf::Color::White);
    
    // Ajouter instructions pour changer de bibliothèque/jeu
    sf::Text libInstructionsText(_font, "7: Next Game   9: Next Graphics", 16);
    libInstructionsText.setFillColor(sf::Color::White);
    
    // Centrer les instructions
    sf::FloatRect instrRect = instructionsText.getLocalBounds();
    centerX = (_width - instrRect.size.x) / 2.0f;
    instructionsText.setPosition(sf::Vector2f(centerX, _height - 80.0f));
    _window.draw(instructionsText);
    
    // Positionner les instructions supplémentaires
    sf::FloatRect libInstrRect = libInstructionsText.getLocalBounds();
    centerX = (_width - libInstrRect.size.x) / 2.0f;
    libInstructionsText.setPosition(sf::Vector2f(centerX, _height - 60.0f));
    _window.draw(libInstructionsText);
    
    // Dessiner une ligne décorative
    sf::RectangleShape decorLine(sf::Vector2f(300.0f, 2.0f));
    decorLine.setFillColor(sf::Color::Cyan);
    decorLine.setPosition(sf::Vector2f((_width - 300.0f) / 2.0f, _height - 100.0f));
    _window.draw(decorLine);
    
    // Afficher le numéro de version en bas à droite
    sf::Text versionText(_font, "v1.0", 14);
    versionText.setFillColor(sf::Color(150, 150, 150));
    versionText.setPosition(sf::Vector2f(_width - 60.0f, _height - 40.0f));
    _window.draw(versionText);
    
    // Stocker seulement le strict minimum dans les collections
    RectDrawElement mainFrameElem{frame};
    _rectElements.push_back(mainFrameElem);
    
    // Afficher immédiatement le menu
    _window.display();
}

// Méthode pour gérer les touches et mettre à jour la sélection du menu
int SFMLGraphics::handleKeyPress(sf::Keyboard::Key key)
{
    const int menuItemCount = 4; // Nombre d'éléments dans le menu
    
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

} // namespace arcd

extern "C" {
    arcd::IGraphicsLibrary* createGraphicsLibrary()
    {
        return new arcd::SFMLGraphics();
    }

    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* graphicsLib)
    {
        delete graphicsLib;
    }
} 