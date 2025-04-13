#include "graphics/LibcacaGraphics.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>

namespace arcd {

LibcacaGraphics::LibcacaGraphics() : canvas(nullptr), display(nullptr), windowWidth(120), windowHeight(40) {
    initColorMap();
}

LibcacaGraphics::~LibcacaGraphics() {
    cleanup();
}

std::string LibcacaGraphics::getName() const {
    return "libcaca";
}

bool LibcacaGraphics::initialize() {
    try {
        canvas = caca_create_canvas(windowWidth, windowHeight);
        if (!canvas) {
            throw std::runtime_error("Failed to create libcaca canvas");
        }

        display = caca_create_display(canvas);
        if (!display) {
            caca_free_canvas(canvas);
            throw std::runtime_error("Failed to create libcaca display");
        }

        // Set initial window title
        caca_set_display_title(display, "Arcade - libcaca");

        // Show splash screen
        clear();
        caca_set_color_ansi(canvas, CACA_WHITE, CACA_BLACK);
        caca_put_str(canvas, windowWidth/2 - 5, windowHeight/2, "ARCADE");
        caca_refresh_display(display);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error initializing libcaca: " << e.what() << std::endl;
        return false;
    }
}

void LibcacaGraphics::cleanup() {
    if (display) {
        caca_free_display(display);
        display = nullptr;
    }
    if (canvas) {
        caca_free_canvas(canvas);
        canvas = nullptr;
    }
}

void LibcacaGraphics::clear() {
    if (canvas) {
        caca_clear_canvas(canvas);
    }
}

void LibcacaGraphics::refresh() {
    if (display) {
        caca_refresh_display(display);
    }
}

void LibcacaGraphics::drawText(int x, int y, const std::string& text, Color color) {
    if (!canvas) return;
    caca_set_color_ansi(canvas, getColor(color), CACA_BLACK);
    caca_put_str(canvas, x, y, text.c_str());
}

void LibcacaGraphics::drawBox(int x, int y, int width, int height, Color color) {
    if (!canvas) return;
    caca_set_color_ansi(canvas, getColor(color), CACA_BLACK);
    
    // Draw corners
    caca_put_char(canvas, x, y, '+');
    caca_put_char(canvas, x + width - 1, y, '+');
    caca_put_char(canvas, x, y + height - 1, '+');
    caca_put_char(canvas, x + width - 1, y + height - 1, '+');
    
    // Draw horizontal lines
    for (int i = x + 1; i < x + width - 1; ++i) {
        caca_put_char(canvas, i, y, '-');
        caca_put_char(canvas, i, y + height - 1, '-');
    }
    
    // Draw vertical lines
    for (int i = y + 1; i < y + height - 1; ++i) {
        caca_put_char(canvas, x, i, '|');
        caca_put_char(canvas, x + width - 1, i, '|');
    }
}

void LibcacaGraphics::drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color) {
    if (!canvas) return;
    
    for (size_t i = 0; i < items.size(); ++i) {
        if (static_cast<int>(i) == selectedIndex) {
            caca_set_color_ansi(canvas, CACA_WHITE, CACA_BLUE);
        } else {
            caca_set_color_ansi(canvas, getColor(color), CACA_BLACK);
        }
        caca_put_str(canvas, x, y + static_cast<int>(i), items[i].c_str());
    }
}

void LibcacaGraphics::drawMenu(
    const std::string& title,
    const std::vector<std::string>& games,
    const std::vector<std::string>& graphics,
    const std::string& playerName,
    int selectedGame,
    int selectedGraphic,
    int selectedMenu,
    int score
) {
    if (!canvas) return;
    
    clear();
    
    // Draw title and player info
    caca_set_color_ansi(canvas, CACA_WHITE, CACA_BLACK);
    std::string decoratedTitle = "=== " + title + " ===";
    caca_put_str(canvas, windowWidth/2 - static_cast<int>(decoratedTitle.length())/2, 2, decoratedTitle.c_str());
    std::string playerInfo = "Player: " + playerName + " | Score: " + std::to_string(score);
    caca_put_str(canvas, windowWidth/2 - static_cast<int>(playerInfo.length())/2, 4, playerInfo.c_str());
    
    // Box dimensions
    const int boxWidth = 35;
    const int boxHeight = 15;
    const int spacing = 6;
    const int startX = (windowWidth - (3 * boxWidth + 2 * spacing)) / 2;
    const int startY = 7;
    
    // Draw boxes and their content
    const int boxPositions[3] = {
        startX,
        startX + boxWidth + spacing,
        startX + 2 * (boxWidth + spacing)
    };
    
    const char* boxTitles[3] = {"Games", "Graphics", "Player"};
    const std::vector<std::string>* boxContents[3] = {&games, &graphics, nullptr};
    const int selectedIndices[3] = {selectedGame, selectedGraphic, 0};
    
    for (int box = 0; box < 3; ++box) {
        bool isSelected = (selectedMenu == box);
        int x = boxPositions[box];
        
        // Draw box
        drawBox(x, startY, boxWidth, boxHeight, isSelected ? Color::YELLOW : Color::WHITE);
        
        // Draw title
        caca_set_color_ansi(canvas, isSelected ? CACA_YELLOW : CACA_WHITE, CACA_BLACK);
        caca_put_str(canvas, x + 2, startY, boxTitles[box]);
        
        // Draw content
        if (box < 2) { // Games and Graphics boxes
            const auto& items = *boxContents[box];
            for (size_t i = 0; i < items.size() && i < static_cast<size_t>(boxHeight - 2); ++i) {
                bool isItemSelected = (isSelected && static_cast<int>(i) == selectedIndices[box]);
                std::string displayName = items[i];
                
                // Clean up the name
                size_t prefixPos = displayName.find("arcade_");
                if (prefixPos != std::string::npos) {
                    displayName = displayName.substr(prefixPos + 7);
                }
                size_t extPos = displayName.find(".so");
                if (extPos != std::string::npos) {
                    displayName = displayName.substr(0, extPos);
                }
                
                if (isItemSelected) {
                    caca_set_color_ansi(canvas, CACA_YELLOW, CACA_BLUE);
                    caca_put_str(canvas, x + 2, startY + 2 + static_cast<int>(i), ("> " + displayName).c_str());
                } else {
                    caca_set_color_ansi(canvas, CACA_WHITE, CACA_BLACK);
                    caca_put_str(canvas, x + 2, startY + 2 + static_cast<int>(i), displayName.c_str());
                }
            }
        } else { // Player box
            const char* playerOptions[] = {"Change Nickname", "Leaderboard"};
            for (int i = 0; i < 2; ++i) {
                bool isItemSelected = (isSelected && i == 0);
                if (isItemSelected) {
                    caca_set_color_ansi(canvas, CACA_YELLOW, CACA_BLUE);
                    caca_put_str(canvas, x + 2, startY + 2 + i, ("> " + std::string(playerOptions[i])).c_str());
                } else {
                    caca_set_color_ansi(canvas, CACA_WHITE, CACA_BLACK);
                    caca_put_str(canvas, x + 2, startY + 2 + i, playerOptions[i]);
                }
            }
        }
    }
    
    // Draw instructions
    const int instructionY = startY + boxHeight + 2;
    drawBox(startX, instructionY, 3 * boxWidth + 2 * spacing, 9, Color::BLUE);
    
    caca_set_color_ansi(canvas, CACA_CYAN, CACA_BLACK);
    caca_put_str(canvas, windowWidth/2 - 4, instructionY, "CONTROLS");
    
    const char* instructions[][2] = {
        {"SELECT", "TAB key"},
        {"NAVIGATE", "Arrow keys"},
        {"CONFIRM", "Enter key"},
        {"EXIT", "Escape key"}
    };
    
    for (int i = 0; i < 4; ++i) {
        int y = instructionY + 2 + i * 2;
        caca_set_color_ansi(canvas, CACA_YELLOW, CACA_BLACK);
        caca_put_str(canvas, startX + 2, y, (std::string(instructions[i][0]) + ":").c_str());
        caca_set_color_ansi(canvas, CACA_WHITE, CACA_BLACK);
        caca_put_str(canvas, startX + 12, y, instructions[i][1]);
    }
    
    refresh();
}

int LibcacaGraphics::getKey() {
    if (!display) return 0;

    while (caca_get_event(display, CACA_EVENT_KEY_PRESS, &event, 0)) {
        int key = caca_get_event_key_ch(&event);
        switch (key) {
            case 'q': return KEY_ESC_CODE;
            case 'z': return KEY_UP_CODE;
            case 's': return KEY_DOWN_CODE;
            case 'd': return KEY_RIGHT_CODE;
            case 'a': return KEY_LEFT_CODE;
            case ' ': return KEY_SPACE_CODE;
            case CACA_KEY_UP: return KEY_UP_CODE;
            case CACA_KEY_DOWN: return KEY_DOWN_CODE;
            case CACA_KEY_LEFT: return KEY_LEFT_CODE;
            case CACA_KEY_RIGHT: return KEY_RIGHT_CODE;
            case CACA_KEY_RETURN: return KEY_ENTER_CODE;
            case CACA_KEY_ESCAPE: return KEY_ESC_CODE;
            case '9': return KEY_NEXT_LIB_CODE;
            case '7': return KEY_NEXT_GAME_CODE;
            case 'r': return KEY_RESTART_GAME;
            default: return key;
        }
    }
    return 0;
}

void LibcacaGraphics::getPlayerName(std::string& playerName) {
    if (!display) return;
    
    clear();
    caca_set_color_ansi(canvas, CACA_WHITE, CACA_BLACK);
    caca_put_str(canvas, windowWidth/2 - 10, windowHeight/2 - 1, "Enter your name: ");
    caca_put_str(canvas, windowWidth/2 - 10, windowHeight/2, playerName.c_str());
    refresh();
    
    while (true) {
        int key = getKey();
        if (key == KEY_ENTER_CODE) {
            break;
        } else if (key == KEY_BACKSPACE_CODE && !playerName.empty()) {
            playerName.pop_back();
        } else if (isprint(key) && playerName.length() < 20) {
            playerName += static_cast<char>(key);
        }
        
        clear();
        caca_put_str(canvas, windowWidth/2 - 10, windowHeight/2 - 1, "Enter your name: ");
        caca_put_str(canvas, windowWidth/2 - 10, windowHeight/2, playerName.c_str());
        refresh();
    }
}

int LibcacaGraphics::getWidth() const {
    return windowWidth;
}

int LibcacaGraphics::getHeight() const {
    return windowHeight;
}

void LibcacaGraphics::initColorMap() {
    colorMap[Color::BLACK] = CACA_BLACK;
    colorMap[Color::RED] = CACA_RED;
    colorMap[Color::GREEN] = CACA_GREEN;
    colorMap[Color::YELLOW] = CACA_YELLOW;
    colorMap[Color::BLUE] = CACA_BLUE;
    colorMap[Color::MAGENTA] = CACA_MAGENTA;
    colorMap[Color::CYAN] = CACA_CYAN;
    colorMap[Color::WHITE] = CACA_WHITE;
    colorMap[Color::DEFAULT] = CACA_WHITE;
}

uint32_t LibcacaGraphics::getColor(Color color) const {
    auto it = colorMap.find(color);
    if (it != colorMap.end()) {
        return it->second;
    }
    return CACA_WHITE;
}

} // namespace arcd

extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary() {
        return std::make_unique<arcd::LibcacaGraphics>();
    }

    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* library) {
        delete library;
    }
} 