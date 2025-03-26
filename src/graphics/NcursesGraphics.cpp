/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** NcursesGraphics
*/

#include "graphics/NcursesGraphics.hpp"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <cstring>

namespace arcd {

// Key constants
constexpr int KEY_UP_CODE = 65;     // Up arrow
constexpr int KEY_DOWN_CODE = 66;   // Down arrow
constexpr int KEY_RIGHT_CODE = 67;  // Right arrow
constexpr int KEY_LEFT_CODE = 68;   // Left arrow
constexpr int KEY_ENTER_CODE = 10;  // Enter key
constexpr int KEY_ESC = 27;         // Escape key
constexpr int KEY_SPACE = 32;       // Space key

NcursesGraphics::NcursesGraphics() : _initialized(false), _width(80), _height(24), _lastKey(0), _frameCounter(0)
{
    // Get terminal size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    _width = w.ws_col;
    _height = w.ws_row;
    
    // Save original terminal settings
    tcgetattr(STDIN_FILENO, &_oldTermios);
    
    // Configure terminal for raw input
    struct termios raw = _oldTermios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

NcursesGraphics::~NcursesGraphics()
{
    cleanup();
    
    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &_oldTermios);
}

bool NcursesGraphics::initialize()
{
    if (_initialized) {
        return true;
    }
    
    _initialized = true;
    
    // Clear screen
    std::cout << "\033[2J\033[H";
    
    // Get terminal size again to ensure it's up to date
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    _width = w.ws_col;
    _height = w.ws_row;
    
    // Big arcade logo, properly centered
    std::string logo[] = {
        "╔═════════════════════════════════════════════╗",
        "║                                             ║",
        "║   █████╗ ██████╗  ██████╗ █████╗ ██████╗ ███████╗   ║",
        "║  ██╔══██╗██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔════╝   ║",
        "║  ███████║██████╔╝██║     ███████║██║  ██║█████╗     ║",
        "║  ██╔══██║██╔══██╗██║     ██╔══██║██║  ██║██╔══╝     ║",
        "║  ██║  ██║██║  ██║╚██████╗██║  ██║██████╔╝███████╗   ║",
        "║  ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚═════╝ ╚══════╝   ║",
        "║                                             ║",
        "╚═════════════════════════════════════════════╝"
    };
    
    // Calculate center positions
    int logoHeight = sizeof(logo) / sizeof(logo[0]);
    int logoWidth = static_cast<int>(logo[0].length());
    int startY = (_height - logoHeight - 4) / 2;
    
    // Draw the centered logo
    std::cout << "\033[1;33m"; // Yellow
    for (int i = 0; i < logoHeight; i++) {
        int startX = (_width - logoWidth) / 2;
        drawText(startX, startY + i, logo[i]);
    }
    
    // Display welcome message
    std::cout << "\033[1;36m"; // Cyan
    std::string welcome = "Welcome to the Arcade System!";
    drawText((_width - static_cast<int>(welcome.length())) / 2, startY + logoHeight + 1, welcome);
    
    std::string pressKey = "Press any key to start...";
    drawText((_width - static_cast<int>(pressKey.length())) / 2, startY + logoHeight + 2, pressKey);
    std::cout << "\033[0m"; // Reset
    
    // Control instructions
    std::cout << "\033[1;37m"; // White
    std::string controls = "Controls: ARROWS=Move | 9=Next lib | 7=Next game | R=Restart | Q=Menu | E=Exit";
    drawText((_width - static_cast<int>(controls.length())) / 2, _height - 2, controls);
    std::cout << "\033[0m"; // Reset
    
    std::cout.flush();
    
    // Wait for a keypress
    std::this_thread::sleep_for(std::chrono::seconds(1));
    flushInputBuffer();
    
    return true;
}

void NcursesGraphics::cleanup()
{
    _initialized = false;
}

void NcursesGraphics::initColors()
{
    // No implementation needed
}

int NcursesGraphics::getColorPair(int fg, [[maybe_unused]] int bg)
{
    return fg;
}

void NcursesGraphics::clear()
{
    if (_initialized) {
        std::cout << "\033[2J\033[H";
    }
}

void NcursesGraphics::refresh()
{
    if (_initialized) {
        std::cout.flush();
        _frameCounter++;
    }
}

void NcursesGraphics::drawText(int x, int y, const std::string& text)
{
    if (!_initialized) {
        return;
    }
    
    std::cout << "\033[" << (y+1) << ";" << (x+1) << "H" << text;
}

void NcursesGraphics::drawBox(int x, int y, int width, int height)
{
    if (!_initialized) {
        return;
    }
    
    // Top border
    std::cout << "\033[" << (y+1) << ";" << (x+1) << "H╔";
    for (int i = 0; i < width - 2; i++) std::cout << "═";
    std::cout << "╗";
    
    // Sides
    for (int i = 1; i < height - 1; i++) {
        std::cout << "\033[" << (y+i+1) << ";" << (x+1) << "H║";
        std::cout << "\033[" << (y+i+1) << ";" << (x+width) << "H║";
    }
    
    // Bottom border
    std::cout << "\033[" << (y+height) << ";" << (x+1) << "H╚";
    for (int i = 0; i < width - 2; i++) std::cout << "═";
    std::cout << "╝";
}

void NcursesGraphics::drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex)
{
    if (!_initialized) {
        return;
    }
    
    // Menu constants
    const int boxWidth = 50;
    const int boxHeight = static_cast<int>(items.size()) + 6;
    
    // Center on screen if x,y are 0
    int menuX = x > 0 ? x : (_width - boxWidth) / 2;
    int menuY = y > 0 ? y : (_height - boxHeight) / 2;
    
    // Draw menu box with double border for more aesthetics
    std::cout << "\033[" << (menuY+1) << ";" << (menuX+1) << "H╔";
    for (int i = 0; i < boxWidth - 2; i++) std::cout << "═";
    std::cout << "╗";
    
    // Menu title
    std::cout << "\033[" << (menuY+2) << ";" << (menuX+1) << "H║";
    std::cout << "\033[1;36m"; // Cyan
    std::string title = " ARCADE MENU ";
    int titleX = (boxWidth - static_cast<int>(title.length())) / 2;
    for (int i = 0; i < titleX; i++) std::cout << " ";
    std::cout << title;
    for (int i = 0; i < boxWidth - 2 - titleX - static_cast<int>(title.length()); i++) std::cout << " ";
    std::cout << "\033[0m"; // Reset
    std::cout << "║";
    
    // Separator
    std::cout << "\033[" << (menuY+3) << ";" << (menuX+1) << "H╠";
    for (int i = 0; i < boxWidth - 2; i++) std::cout << "═";
    std::cout << "╣";
    
    // Menu items
    for (size_t i = 0; i < items.size(); i++) {
        int itemY = menuY + 4 + static_cast<int>(i);
        std::cout << "\033[" << itemY << ";" << (menuX+1) << "H║";
        
        // Basic box for each item
        if (static_cast<int>(i) == selectedIndex) {
            // Highlighted item - with animation based on frame counter
            std::string prefix;
            if (_frameCounter % 6 < 3) {
                prefix = "►► ";
                std::cout << "\033[1;33m"; // Bright yellow
            } else {
                prefix = " ► ";
                std::cout << "\033[1;32m"; // Bright green
            }
            
            // Centered item text
            int textLen = static_cast<int>(items[i].length() + prefix.length());
            int padding = (boxWidth - 2 - textLen) / 2;
            for (int j = 0; j < padding; j++) std::cout << " ";
            std::cout << prefix << items[i];
            for (int j = 0; j < boxWidth - 2 - padding - textLen; j++) std::cout << " ";
            std::cout << "\033[0m"; // Reset
        } else {
            // Regular item - centered
            int padding = (boxWidth - 2 - static_cast<int>(items[i].length())) / 2;
            for (int j = 0; j < padding; j++) std::cout << " ";
            std::cout << items[i];
            for (int j = 0; j < boxWidth - 2 - padding - static_cast<int>(items[i].length()); j++) std::cout << " ";
        }
        std::cout << "║";
    }
    
    // Extra space before bottom border
    std::cout << "\033[" << (menuY+4+static_cast<int>(items.size())) << ";" << (menuX+1) << "H║";
    for (int i = 0; i < boxWidth - 2; i++) std::cout << " ";
    std::cout << "║";
    
    // Bottom border
    std::cout << "\033[" << (menuY+5+static_cast<int>(items.size())) << ";" << (menuX+1) << "H╚";
    for (int i = 0; i < boxWidth - 2; i++) std::cout << "═";
    std::cout << "╝";
    
    // Draw control instructions
    std::cout << "\033[" << (menuY+boxHeight) << ";" << menuX << "H";
    std::cout << "\033[1;37m"; // White
    std::string controls = "↑/↓: Navigate | Enter: Select | Q: Menu | E: Exit";
    int controlsX = (boxWidth - static_cast<int>(controls.length())) / 2;
    for (int i = 0; i < controlsX; i++) std::cout << " ";
    std::cout << controls;
    std::cout << "\033[0m"; // Reset
    
    // Additional controls below
    std::cout << "\033[" << (menuY+boxHeight+1) << ";" << menuX << "H";
    std::cout << "\033[1;37m"; // White
    std::string gameControls = "9: Next Graphics | 7: Next Game | R: Restart";
    int gameControlsX = (boxWidth - static_cast<int>(gameControls.length())) / 2;
    for (int i = 0; i < gameControlsX; i++) std::cout << " ";
    std::cout << gameControls;
    std::cout << "\033[0m"; // Reset
}

void NcursesGraphics::flushInputBuffer()
{
    tcflush(STDIN_FILENO, TCIFLUSH);
    _lastKey = 0;
}

int NcursesGraphics::getKey()
{
    if (!_initialized) {
        return 0;
    }
    
    // Check for input with a minimal delay
    fd_set readfds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 10000; // 10ms
    
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    
    if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0) {
        char buf[3] = {0};
        int n = read(STDIN_FILENO, buf, sizeof(buf));
        
        if (n > 0) {
            if (buf[0] == KEY_ESC && n > 2) {
                // Arrow keys (Escape sequence)
                if (buf[1] == '[') {
                    switch (buf[2]) {
                        case KEY_UP_CODE: return KEY_UP; // Up arrow
                        case KEY_DOWN_CODE: return KEY_DOWN; // Down arrow
                        case KEY_RIGHT_CODE: return KEY_RIGHT; // Right arrow
                        case KEY_LEFT_CODE: return KEY_LEFT; // Left arrow
                    }
                }
                return 0; // Unknown escape sequence
            } else if (n == 1) {
                // Map specific keys
                switch (buf[0]) {
                    case '7': return '7'; // Next game
                    case '9': return '9'; // Next graphics
                    case 'r':
                    case 'R': return 'r'; // Restart
                    case 'q':
                    case 'Q': return 'q'; // Menu
                    case 'e':
                    case 'E': return 'e'; // Exit
                    default: return buf[0]; // Other keys
                }
            }
        }
    }
    
    return 0; // No key pressed
}

std::string NcursesGraphics::getName() const
{
    return "Ncurses";
}

} // namespace arcd

// Export C functions for dynamic loading
extern "C" {
    arcd::IGraphicsLibrary* createGraphicsLibrary() {
        return new arcd::NcursesGraphics();
    }
    
    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* graphicsLib) {
        delete graphicsLib;
    }
}