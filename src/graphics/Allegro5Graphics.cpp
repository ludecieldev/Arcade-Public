/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** Allegro5Graphics
*/

#include "graphics/Allegro5Graphics.hpp"
#include <iostream>
#include <filesystem>

namespace arcd {

Allegro5Graphics::Allegro5Graphics()
    : _initialized(false)
    , _width(1024)
    , _height(768)
    , _playerName("Player")
    , _lastKey(-1)
    , _frameCounter(0)
    , _display(nullptr, al_destroy_display)
    , _eventQueue(nullptr, al_destroy_event_queue)
    , _font(nullptr, al_destroy_font)
    , _timer(nullptr, al_destroy_timer)
    , _shouldClose(false)
{
}

Allegro5Graphics::~Allegro5Graphics() {
    cleanup();
}

/**
 * @brief Initialize the graphics library
 * - Check if already initialized
 * - Initialize Allegro5
 * - Install necessary addons
 * - Create display window
 * - Create event queue
 * - Create timer
 * - Register event sources
 * - Load font
 * - Initialize colors
 * - Start timer
 * 
 * @return true if initialization succeeded, false otherwise
 */
bool Allegro5Graphics::initialize() {
    if (_initialized) {
        return true;
    }

    if (!al_init()) {
        std::cerr << "Failed to initialize Allegro5" << std::endl;
        return false;
    }

    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_install_keyboard();
    
    al_set_keyboard_leds(0);

    _display.reset(al_create_display(_width, _height));
    if (!_display) {
        std::cerr << "Failed to create display" << std::endl;
        return false;
    }

    _eventQueue.reset(al_create_event_queue());
    if (!_eventQueue) {
        std::cerr << "Failed to create event queue" << std::endl;
        return false;
    }

    _timer.reset(al_create_timer(1.0 / 30.0));
    if (!_timer) {
        std::cerr << "Failed to create timer" << std::endl;
        return false;
    }

    al_register_event_source(_eventQueue.get(), al_get_display_event_source(_display.get()));
    al_register_event_source(_eventQueue.get(), al_get_keyboard_event_source());
    al_register_event_source(_eventQueue.get(), al_get_timer_event_source(_timer.get()));

    std::string fontPath = "assets/fonts/Arial.ttf";
    if (!std::filesystem::exists(fontPath)) {
        std::cerr << "Font file not found: " << fontPath << std::endl;
        return false;
    }

    _font.reset(al_load_ttf_font(fontPath.c_str(), 24, 0));
    if (!_font) {
        std::cerr << "Failed to load font" << std::endl;
        return false;
    }

    initColors();

    al_start_timer(_timer.get());

    _initialized = true;
    _shouldClose = false;
    return true;
}

/**
 * @brief Clean up resources used by the graphics library
 * - Reset all resources in reverse order of creation
 * - Mark library as uninitialized
 */
void Allegro5Graphics::cleanup() {
    _timer.reset();
    _eventQueue.reset();
    _font.reset();
    _display.reset();
    _initialized = false;
}

/**
 * @brief Clear the screen to black
 */
void Allegro5Graphics::clear() {
    if (!_initialized) return;
    al_clear_to_color(al_map_rgb(0, 0, 0));
}

/**
 * @brief Refresh the display (swap buffers)
 */
void Allegro5Graphics::refresh() {
    if (!_initialized) return;
    al_flip_display();
}

/**
 * @brief Get the last key pressed
 * - Check if initialized
 * - Process events from the event queue
 * - Handle special keys and character keys
 * - Map Allegro key codes to application key codes
 * 
 * @return integer code of the key or -1 if no key pressed
 */
int Allegro5Graphics::getKey()
{
    if (!_initialized) return -1;
    
    ALLEGRO_EVENT event;
    while (al_get_next_event(_eventQueue.get(), &event)) {
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            _shouldClose = true;
            return KEY_ESC_CODE;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (event.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    return KEY_UP_CODE;
                case ALLEGRO_KEY_DOWN:
                    return KEY_DOWN_CODE;
                case ALLEGRO_KEY_LEFT:
                    return KEY_LEFT_CODE;
                case ALLEGRO_KEY_RIGHT:
                    return KEY_RIGHT_CODE;
                case ALLEGRO_KEY_ENTER:
                    return KEY_ENTER_CODE;
                case ALLEGRO_KEY_ESCAPE:
                    _shouldClose = true;
                    return KEY_ESC_CODE;
                case ALLEGRO_KEY_SPACE:
                    return KEY_SPACE_CODE;
                case ALLEGRO_KEY_TAB:
                    return KEY_TAB_CODE;
                case ALLEGRO_KEY_BACKSPACE:
                    return KEY_BACKSPACE_CODE;
                case ALLEGRO_KEY_9:
                    return KEY_NEXT_LIB_CODE;
                case ALLEGRO_KEY_7:
                    return KEY_NEXT_GAME_CODE;
                case ALLEGRO_KEY_R:
                    return KEY_RESTART_GAME;
                default:
                    if (event.keyboard.keycode >= ALLEGRO_KEY_A && event.keyboard.keycode <= ALLEGRO_KEY_Z) {
                        if (event.keyboard.keycode == ALLEGRO_KEY_R) {
                            return KEY_RESTART_GAME;
                        }
                        return 'a' + (event.keyboard.keycode - ALLEGRO_KEY_A);
                    }
                    else if (event.keyboard.keycode >= ALLEGRO_KEY_0 && event.keyboard.keycode <= ALLEGRO_KEY_9) {
                        return '0' + (event.keyboard.keycode - ALLEGRO_KEY_0);
                    }
                    else if (event.keyboard.unichar > 0) {
                        return event.keyboard.unichar;
                    }
                    break;
            }
        }
    }
    return -1;
}

/**
 * @brief Get the player's name via user input
 * - Return current name if library not initialized
 * - Create a dialog for name editing
 * - Handle keyboard input for editing
 * - Support for blinking cursor
 * - Validate name before accepting
 * - Clean up resources after name input
 * 
 * @param playerName reference to string to store the player name
 */
void Allegro5Graphics::getPlayerName(std::string& playerName) {
    if (!_initialized) {
        playerName = _playerName;
        return;
    }
    
    std::string newName = _playerName;
    bool done = false;
    bool redraw = true;
    
    if (!al_is_keyboard_installed()) {
        al_install_keyboard();
    }
    
    bool cursorVisible = true;
    
    std::unique_ptr<ALLEGRO_TIMER, void(*)(ALLEGRO_TIMER*)> cursorTimer(al_create_timer(0.5), al_destroy_timer);
    if (!cursorTimer) {
        playerName = _playerName;
        return;
    }
    
    std::unique_ptr<ALLEGRO_EVENT_QUEUE, void(*)(ALLEGRO_EVENT_QUEUE*)> nameEventQueue(al_create_event_queue(), al_destroy_event_queue);
    if (!nameEventQueue) {
        playerName = _playerName;
        return;
    }
    
    al_register_event_source(nameEventQueue.get(), al_get_keyboard_event_source());
    al_register_event_source(nameEventQueue.get(), al_get_timer_event_source(cursorTimer.get()));
    al_register_event_source(nameEventQueue.get(), al_get_display_event_source(_display.get()));
    
    al_start_timer(cursorTimer.get());
    
    std::unique_ptr<ALLEGRO_FONT, void(*)(ALLEGRO_FONT*)> nameFont(al_load_ttf_font("assets/fonts/Arial.ttf", 36, 0), al_destroy_font);
    if (!nameFont) {
        nameFont.reset(_font.get());
    }
    
    while (!done) {
        ALLEGRO_EVENT event;
        
        if (redraw) {
            redraw = false;
            
            clear();
            
            al_draw_filled_rectangle(0, 0, _width, _height, al_map_rgba(0, 0, 0, 200));
            
            al_draw_filled_rectangle(
                _width/4, _height/3,
                _width*3/4, _height*2/3,
                al_map_rgb(30, 30, 30)
            );
            
            al_draw_rectangle(
                _width/4, _height/3,
                _width*3/4, _height*2/3,
                al_map_rgb(200, 200, 200), 2.0f
            );
            
            al_draw_text(
                nameFont.get(), al_map_rgb(255, 255, 255),
                _width/2, _height/3 + 30,
                ALLEGRO_ALIGN_CENTER, "Enter Player Name"
            );
            
            std::string displayText = newName;
            if (cursorVisible) {
                displayText += "_";
            }
            
            al_draw_text(
                nameFont.get(), al_map_rgb(255, 255, 0),
                _width/2, _height/2 - 18,
                ALLEGRO_ALIGN_CENTER, displayText.c_str()
            );
            
            al_draw_text(
                _font.get(), al_map_rgb(200, 200, 200),
                _width/2, _height*2/3 - 40,
                ALLEGRO_ALIGN_CENTER, "Press ENTER to confirm, ESC to cancel"
            );
            
            refresh();
        }
        
        bool hasEvent = al_get_next_event(nameEventQueue.get(), &event);
        
        if (!hasEvent) {
            continue;
        }
        
        if (event.type == ALLEGRO_EVENT_TIMER) {
            if (event.timer.source == cursorTimer.get()) {
                cursorVisible = !cursorVisible;
                redraw = true;
            }
        }
        else if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
            int keycode = event.keyboard.keycode;
            int unichar = event.keyboard.unichar;
            
            if (keycode == ALLEGRO_KEY_ESCAPE) {
                done = true;
            }
            else if (keycode == ALLEGRO_KEY_ENTER || keycode == ALLEGRO_KEY_PAD_ENTER) {
                if (!newName.empty()) {
                    _playerName = newName;
                    playerName = _playerName;
                    done = true;
                }
            }
            else if (keycode == ALLEGRO_KEY_BACKSPACE) {
                if (!newName.empty()) {
                    newName.pop_back();
                    redraw = true;
                }
            }
            else if (unichar >= 32 && unichar < 127 && newName.length() < 20) {
                newName += static_cast<char>(unichar);
                redraw = true;
            }
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            done = true;
        }
    }
    
    al_stop_timer(cursorTimer.get());
    
    playerName = _playerName;
    
    ALLEGRO_EVENT flushEvent;
    while (al_get_next_event(_eventQueue.get(), &flushEvent)) {
    }
    
    al_rest(0.2);
}

/**
 * @brief Get the name of the graphics library
 * 
 * @return name of the graphics library
 */
std::string Allegro5Graphics::getName() const {
    return "Allegro5";
}

/**
 * @brief Get the width of the display
 * 
 * @return width in pixels
 */
int Allegro5Graphics::getWidth() const {
    return _width;
}

/**
 * @brief Get the height of the display
 * 
 * @return height in pixels
 */
int Allegro5Graphics::getHeight() const {
    return _height;
}

/**
 * @brief Draw text at the specified position
 * - Check if library is initialized
 * - Calculate line height for spacing
 * - Split text by newlines and draw each line
 * 
 * @param x x-coordinate
 * @param y y-coordinate
 * @param text the text to draw
 * @param color color to use for drawing
 */
void Allegro5Graphics::drawText(int x, int y, const std::string& text, Color color) {
    if (!_initialized || !_font) return;
    
    int lineHeight = al_get_font_line_height(_font.get());
    
    std::string line;
    size_t pos = 0;
    int currentY = y;
    
    while (pos < text.length()) {
        size_t newline = text.find('\n', pos);
        if (newline == std::string::npos) {
            line = text.substr(pos);
            al_draw_text(_font.get(), _colors[color], x, currentY, ALLEGRO_ALIGN_LEFT, line.c_str());
            break;
        } else {
            line = text.substr(pos, newline - pos);
            al_draw_text(_font.get(), _colors[color], x, currentY, ALLEGRO_ALIGN_LEFT, line.c_str());
            currentY += lineHeight + 10;
            pos = newline + 1;
        }
    }
}

/**
 * @brief Draw a box at the specified position
 * 
 * @param x x-coordinate
 * @param y y-coordinate
 * @param width width of the box
 * @param height height of the box
 * @param color color to use for drawing
 */
void Allegro5Graphics::drawBox(int x, int y, int width, int height, Color color) {
    if (!_initialized) return;
    al_draw_rectangle(x, y, x + width, y + height, _colors[color], 2.0f);
}

/**
 * @brief Draw a list of items with a selected index
 * - Calculate line height with spacing
 * - Draw each item with appropriate color based on selection
 * 
 * @param x x-coordinate
 * @param y y-coordinate
 * @param items vector of strings to display
 * @param selectedIndex index of the selected item
 * @param color color to use for drawing
 */
void Allegro5Graphics::drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color) {
    if (!_initialized || !_font) return;
    
    int lineHeight = al_get_font_line_height(_font.get()) + 10;
    for (size_t i = 0; i < items.size(); ++i) {
        Color itemColor = (static_cast<int>(i) == selectedIndex) ? Color::YELLOW : color;
        al_draw_text(_font.get(), _colors[itemColor], x, y + (i * lineHeight), ALLEGRO_ALIGN_LEFT, items[i].c_str());
    }
}

/**
 * @brief Initialize color mappings for the library
 * - Map each Color enum value to an ALLEGRO_COLOR
 */
void Allegro5Graphics::initColors() {
    _colors[Color::BLACK] = al_map_rgb(0, 0, 0);
    _colors[Color::RED] = al_map_rgb(255, 0, 0);
    _colors[Color::GREEN] = al_map_rgb(0, 255, 0);
    _colors[Color::YELLOW] = al_map_rgb(255, 255, 0);
    _colors[Color::BLUE] = al_map_rgb(0, 0, 255);
    _colors[Color::MAGENTA] = al_map_rgb(255, 0, 255);
    _colors[Color::CYAN] = al_map_rgb(0, 255, 255);
    _colors[Color::WHITE] = al_map_rgb(255, 255, 255);
    _colors[Color::DEFAULT] = al_map_rgb(255, 255, 255);
}

/**
 * @brief Update window dimensions from the display
 */
void Allegro5Graphics::updateWindowSize() {
    if (_initialized) {
        _width = al_get_display_width(_display.get());
        _height = al_get_display_height(_display.get());
    }
}

/**
 * @brief Draw a test screen to verify graphics functionality
 * - Draw title
 * - Draw menu boxes for games, graphics, and player name
 * - Draw instruction text
 */
void Allegro5Graphics::drawTestScreen() {
    if (!_initialized) return;
    clear();
    
    int titleWidth = al_get_text_width(_font.get(), "ARCADE");
    drawText(_width / 2 - titleWidth / 2, 50, "ARCADE", Color::WHITE);
    
    drawMenuBox("Games", {"Snake", "Nibbler", "Pacman"}, 100, 250, 400, 250, true);
    drawMenuBox("Graphics", {"Allegro5", "SDL2", "Ncurses"}, 350, 150, 200, 250, false);
    drawMenuBox("Player Name", {_playerName}, 600, 150, 200, 250, false);
    
    int instructionY = 420;
    drawText(100, instructionY, "Use TAB to switch between boxes", Color::WHITE);
    instructionY += al_get_font_line_height(_font.get()) + 10;
    drawText(100, instructionY, "Use ARROWS to navigate", Color::WHITE);
    instructionY += al_get_font_line_height(_font.get()) + 10;
    drawText(100, instructionY, "Press ENTER to select", Color::WHITE);
    
    refresh();
}

/**
 * @brief Draw a menu box with title and items
 * - Draw box with appropriate color based on selection
 * - Draw title text
 * - Draw each item with appropriate spacing
 * 
 * @param title title of the box
 * @param items vector of items to display
 * @param x x-coordinate of the box
 * @param y y-coordinate of the box
 * @param width width of the box
 * @param height height of the box
 * @param isSelected whether this box is selected
 */
void Allegro5Graphics::drawMenuBox(const std::string& title, const std::vector<std::string>& items,
                  int x, int y, int width, int height, bool isSelected) {
    Color boxColor = isSelected ? Color::YELLOW : Color::WHITE;
    drawBox(x, y, width, height, boxColor);
    
    drawText(x + 10, y + 20, title, boxColor);
    
    int itemY = y + 60;
    int lineSpacing = al_get_font_line_height(_font.get()) + 10;
    for (const auto& item : items) {
        drawText(x + 10, itemY, item, boxColor);
        itemY += lineSpacing;
    }
}

/**
 * @brief Draw the menu screen
 * - Draw title with larger font
 * - Calculate positions for menu boxes
 * - Process game and graphics names for clean display
 * - Draw game options box
 * - Draw graphics options box
 * - Draw player options box
 * - Draw instruction boxes
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
void Allegro5Graphics::drawMenu(
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
    
    std::unique_ptr<ALLEGRO_FONT, void(*)(ALLEGRO_FONT*)> titleFont(al_load_ttf_font("assets/fonts/Arial.ttf", 36, 0), al_destroy_font);
    if (titleFont) {
        al_draw_text(titleFont.get(), _colors[Color::WHITE], _width / 2, 50, ALLEGRO_ALIGN_CENTER, title.c_str());
    } else {
        int titleWidth = al_get_text_width(_font.get(), title.c_str());
        drawText(_width / 2 - titleWidth / 2, 50, title, Color::WHITE);
    }
    
    const int boxWidth = 250;
    const int boxHeight = 300;
    const int margin = 50;
    const int boxesY = 120;
    
    const int totalWidth = 3 * boxWidth + 2 * margin;
    const int startX = (_width - totalWidth) / 2;
    
    bool isGameSelected = (selectedMenu == 0);
    bool isGraphicsSelected = (selectedMenu == 1);
    bool isPlayerSelected = (selectedMenu == 2);

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

    drawBox(startX, boxesY, boxWidth, boxHeight, isGameSelected ? Color::YELLOW : Color::WHITE);
    drawText(startX + 10, boxesY + 20, "Games", isGameSelected ? Color::YELLOW : Color::WHITE);
    int itemY = boxesY + 60;
    int lineSpacing = al_get_font_line_height(_font.get()) + 15;
    for (size_t i = 0; i < displayGameNames.size(); i++) {
        Color itemColor = (isGameSelected && static_cast<int>(i) == selectedGameIndex) ? Color::YELLOW : Color::WHITE;
        drawText(startX + 20, itemY, displayGameNames[i], itemColor);
        itemY += lineSpacing;
    }
    
    drawBox(startX + boxWidth + margin, boxesY, boxWidth, boxHeight, isGraphicsSelected ? Color::YELLOW : Color::WHITE);
    drawText(startX + boxWidth + margin + 10, boxesY + 20, "Graphics", isGraphicsSelected ? Color::YELLOW : Color::WHITE);
    itemY = boxesY + 60;
    for (size_t i = 0; i < displayGraphicNames.size(); i++) {
        Color itemColor = (isGraphicsSelected && static_cast<int>(i) == selectedGraphicIndex) ? Color::YELLOW : Color::WHITE;
        drawText(startX + boxWidth + margin + 20, itemY, displayGraphicNames[i], itemColor);
        itemY += lineSpacing;
    }
    
    drawBox(startX + 2 * (boxWidth + margin), boxesY, boxWidth, boxHeight, isPlayerSelected ? Color::YELLOW : Color::WHITE);
    drawText(startX + 2 * (boxWidth + margin) + 10, boxesY + 20, "Player: " + playerName, isPlayerSelected ? Color::YELLOW : Color::WHITE);
    
    std::vector<std::string> playerOptions = {"Change Nickname", "Leaderboard"};
    itemY = boxesY + 60;
    for (size_t i = 0; i < playerOptions.size(); i++) {
        Color itemColor = (isPlayerSelected && static_cast<int>(i) == playerOptionSelected) ? Color::YELLOW : Color::WHITE;
        drawText(startX + 2 * (boxWidth + margin) + 20, itemY, playerOptions[i], itemColor);
        itemY += lineSpacing;
    }
    
    const int instructionsStartY = boxesY + boxHeight + 50;
    const int instructionHeight = 40;
    const int instructionSpacing = 15;
    
    std::vector<std::pair<std::string, std::string>> instructions = {
        {"SELECT", "Use TAB to switch between boxes"},
        {"NAVIGATE", "Use ARROWS to navigate options"},
        {"CONFIRM", "Press ENTER to select option"},
        {"EXIT", "Press ESC to exit"}
    };
    
    for (size_t i = 0; i < instructions.size(); i++) {
        int y = instructionsStartY + i * (instructionHeight + instructionSpacing);
        
        al_draw_filled_rectangle(
            startX, y, 
            startX + totalWidth, y + instructionHeight, 
            al_map_rgba(50, 50, 50, 150)
        );
        
        drawText(startX + 20, y + 8, instructions[i].first + ":", Color::YELLOW);
        
        drawText(startX + 150, y + 8, instructions[i].second, Color::WHITE);
    }

    refresh();
}

} // namespace arcd

extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary() {
        return std::make_unique<arcd::Allegro5Graphics>();
    }
}
