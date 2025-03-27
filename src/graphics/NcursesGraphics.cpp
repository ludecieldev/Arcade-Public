/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** NcursesGraphics - Terminal-based graphics using ANSI escape sequences
*/

#include "graphics/NcursesGraphics.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>

namespace arcd {

// Define key constants as enum to avoid conflicts
#define KEY_UP 259
#define KEY_DOWN 258
#define KEY_LEFT 260
#define KEY_RIGHT 261
#define KEY_ENTER 10
#define KEY_ESC 27
#define KEY_BACKSPACE 127

NcursesGraphics::NcursesGraphics() : 
    _initialized(false), 
    _width(80), 
    _height(24),
    _lastKey(0),
    _frameCounter(0)
{
    // Get terminal size
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1) {
        _width = ws.ws_col;
        _height = ws.ws_row;
    }
    
    // Save original terminal settings
    tcgetattr(STDIN_FILENO, &_oldTermios);
}

NcursesGraphics::~NcursesGraphics()
{
    cleanup();
}

bool NcursesGraphics::initialize()
{
    if (_initialized) return true;
    
    // Configure terminal for raw input mode
    struct termios newTermios = _oldTermios;
    newTermios.c_lflag &= ~(ICANON | ECHO);
    newTermios.c_cc[VMIN] = 0;
    newTermios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
    
    // Set non-blocking input
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    
    // Hide cursor and clear screen
    std::cout << "\033[?25l" << "\033[2J" << "\033[H";
    std::cout.flush();
    
    _initialized = true;
    
    // Show splash screen
    showSplashScreen();
    
    return true;
}

void NcursesGraphics::cleanup()
{
    if (!_initialized) return;
    
    // Reset terminal
    std::cout << "\033[0m" << "\033[?25h" << "\033[2J" << "\033[H";
    std::cout.flush();
    
    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &_oldTermios);
    
    _initialized = false;
}

void NcursesGraphics::clear()
{
    if (!_initialized) return;
    std::cout << "\033[2J" << "\033[H";
}

void NcursesGraphics::refresh()
{
    if (!_initialized) return;
    std::cout.flush();
    _frameCounter++;
}

void NcursesGraphics::drawText(int x, int y, const std::string& text)
{
    if (!_initialized) return;
    
    // Check boundaries
    if (y < 0 || y >= _height || x < 0) return;
    
    // Move cursor and output text
    std::cout << "\033[" << (y+1) << ";" << (x+1) << "H" << text;
}

void NcursesGraphics::drawBox(int x, int y, int width, int height)
{
    if (!_initialized) return;
    
    // Validate dimensions
    if (width < 2 || height < 2) return;
    
    // Ensure box fits in the screen
    width = std::min(width, _width - x);
    height = std::min(height, _height - y);
    
    if (width < 2 || height < 2) return;
    
    // Top border
    drawText(x, y, "╔");
    for (int i = 1; i < width - 1; i++) {
        drawText(x + i, y, "═");
    }
    drawText(x + width - 1, y, "╗");
    
    // Sides
    for (int j = 1; j < height - 1; j++) {
        drawText(x, y + j, "║");
        drawText(x + width - 1, y + j, "║");
    }
    
    // Bottom border
    drawText(x, y + height - 1, "╚");
    for (int i = 1; i < width - 1; i++) {
        drawText(x + i, y + height - 1, "═");
    }
    drawText(x + width - 1, y + height - 1, "╝");
}

void NcursesGraphics::drawList([[maybe_unused]] int x, [[maybe_unused]] int y, 
                              const std::vector<std::string>& items, int selectedIndex)
{
    if (!_initialized || items.empty()) return;
    
    // Clear first
    clear();
    
    // Title
    std::string title = "ARCADE MENU";
    std::cout << "\033[1;36m"; // Bright cyan
    drawBox(_width/2 - 10, 1, 20, 3);
    drawText(_width/2 - title.length()/2, 2, title);
    std::cout << "\033[0m";
    
    // Menu items with separate boxes
    const int boxWidth = 30;
    const int boxHeight = 3;
    const int spacing = 1;
    const int startY = 6;
    
    for (size_t i = 0; i < items.size(); i++) {
        int itemY = startY + i * (boxHeight + spacing);
        
        if ((int)i == selectedIndex) {
            std::cout << "\033[1;33m"; // Bright yellow for selected
        } else {
            std::cout << "\033[0m"; // Reset for unselected
        }
        
        // Draw box
        drawBox(_width/2 - boxWidth/2, itemY, boxWidth, boxHeight);
        
        // Draw text
        drawText(_width/2 - items[i].length()/2, itemY + 1, items[i]);
    }
    
    // Draw controls
    std::cout << "\033[0m\033[1m"; // Reset and bold
    std::string controls = "↑/↓: Navigate | Enter: Select | Q: Quit";
    drawText(_width/2 - controls.length()/2, _height - 2, controls);
    std::cout << "\033[0m"; // Reset
    
    refresh();
}

void NcursesGraphics::getPlayerName(std::string& playerName)
{
    if (!_initialized) return;
    
    // Clear screen and setup
    clear();
    
    const int boxWidth = 40;
    const int boxHeight = 5;
    const int boxX = _width/2 - boxWidth/2;
    const int boxY = _height/2 - boxHeight/2;
    
    // Title
    std::cout << "\033[1;36m"; // Bright cyan
    drawBox(boxX, boxY - 4, boxWidth, 3);
    std::string title = "ENTER PLAYER NAME";
    drawText(boxX + boxWidth/2 - title.length()/2, boxY - 3, title);
    std::cout << "\033[0m";
    
    // Input box
    std::cout << "\033[1;37m"; // White
    drawBox(boxX, boxY, boxWidth, boxHeight);
    std::cout << "\033[0m";
    
    // Instructions
    std::string instructions = "Enter your name (max 15 chars):";
    drawText(boxX + boxWidth/2 - instructions.length()/2, boxY + 1, instructions);
    
    // Status
    std::string status = "Press ENTER to confirm, ESC to cancel";
    drawText(boxX + boxWidth/2 - status.length()/2, boxY + boxHeight + 2, status);
    
    // Input handling
    const int inputX = boxX + 5;
    const int inputY = boxY + 3;
    std::string input = playerName;
    const size_t maxLength = 15;
    
    bool done = false;
    while (!done) {
        // Display input with cursor
        std::string spaces(maxLength + 1, ' ');
        drawText(inputX, inputY, spaces);
        drawText(inputX, inputY, input + "_");
        refresh();
        
        // Get input
        int key = waitForKey(500); // 500ms timeout
        
        if (key == KEY_ENTER) {
            done = true;
        } else if (key == KEY_ESC) {
            input = playerName; // Restore original
            done = true;
        } else if (key == KEY_BACKSPACE) {
            if (!input.empty()) {
                input.pop_back();
            }
        } else if (key >= 32 && key <= 126) { // Printable ASCII
            if (input.length() < maxLength) {
                input += static_cast<char>(key);
            }
        }
    }
    
    // Update player name if not empty
    if (!input.empty()) {
        playerName = input;
    }
    
    clear();
}

int NcursesGraphics::getKey()
{
    if (!_initialized) return 0;
    
    char buffer[8] = {0};
    int bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
    
    if (bytesRead <= 0) return 0;
    
    // Handle escape sequences for arrow keys
    if (bytesRead >= 3 && buffer[0] == 27 && buffer[1] == '[') {
        switch (buffer[2]) {
            case 'A': return KEY_UP;    // Up arrow
            case 'B': return KEY_DOWN;  // Down arrow
            case 'C': return KEY_RIGHT; // Right arrow
            case 'D': return KEY_LEFT;  // Left arrow
        }
    }
    
    // Handle regular keys
    if (bytesRead == 1) {
        switch (buffer[0]) {
            case 27:  return KEY_ESC;      // ESC
            case 127: return KEY_BACKSPACE; // Backspace
            case 10:  return KEY_ENTER;     // Enter
            default:  return buffer[0];    // Regular character
        }
    }
    
    return 0;
}

void NcursesGraphics::flushInputBuffer()
{
    if (!_initialized) return;
    tcflush(STDIN_FILENO, TCIFLUSH);
}

void NcursesGraphics::initColors()
{
    // Not needed - we use ANSI escape codes
}

int NcursesGraphics::getColorPair(int fg, [[maybe_unused]] int bg)
{
    return fg; // Just return fg - bg is ignored in this implementation
}

std::string NcursesGraphics::getName() const
{
    return "Ncurses";
}

// Helper function for splash screen
void NcursesGraphics::showSplashScreen()
{
    if (!_initialized) return;
    
    clear();
    
    // Draw logo
    std::string logo[] = {
        "╔══════════════════════════════════════════════════════╗",
        "║  █████╗ ██████╗  ██████╗ █████╗ ██████╗ ███████╗     ║",
        "║ ██╔══██╗██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔════╝     ║",
        "║ ███████║██████╔╝██║     ███████║██║  ██║█████╗       ║",
        "║ ██╔══██║██╔══██╗██║     ██╔══██║██║  ██║██╔══╝       ║",
        "║ ██║  ██║██║  ██║╚██████╗██║  ██║██████╔╝███████╗     ║",
        "║ ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚═════╝ ╚══════╝     ║",
        "╚══════════════════════════════════════════════════════╝"
    };
    
    int logoHeight = sizeof(logo) / sizeof(logo[0]);
    int logoY = _height/2 - logoHeight - 5;
    
    // Draw logo with color
    std::cout << "\033[1;33m"; // Bright yellow
    for (int i = 0; i < logoHeight; i++) {
        drawText(_width/2 - logo[i].length()/2, logoY + i, logo[i]);
    }
    
    // Welcome message
    std::cout << "\033[1;36m"; // Bright cyan
    std::string welcome = "Welcome to the Arcade System!";
    drawText(_width/2 - welcome.length()/2, logoY + logoHeight + 2, welcome);
    
    std::string press = "Press any key to start...";
    drawText(_width/2 - press.length()/2, logoY + logoHeight + 4, press);
    
    // Controls
    std::cout << "\033[1;37m"; // Bright white
    std::string controls = "CONTROLS: Arrow Keys=Navigate | Enter=Select | 9=Next Lib | 7=Next Game | R=Restart | Q=Menu | E=Exit";
    drawText(_width/2 - controls.length()/2, _height - 3, controls);
    
    std::cout << "\033[0m"; // Reset colors
    refresh();
    
    // Wait for keypress
    waitForAnyKey();
    
    // Clear screen before returning
    clear();
}

// Helper function to wait for specific key with timeout
int NcursesGraphics::waitForKey(int timeoutMs)
{
    struct pollfd pfd = {STDIN_FILENO, POLLIN, 0};
    int result = poll(&pfd, 1, timeoutMs);
    
    if (result > 0 && (pfd.revents & POLLIN)) {
        return getKey();
    }
    
    return 0;
}

// Helper function to wait for any key
void NcursesGraphics::waitForAnyKey()
{
    flushInputBuffer();
    while (true) {
        if (waitForKey(100) != 0) {
            break;
        }
    }
    flushInputBuffer();
}

} // namespace arcd

extern "C" {
    arcd::IGraphicsLibrary* createGraphicsLibrary() {
        return new arcd::NcursesGraphics();
    }
    
    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* graphicsLib) {
        delete graphicsLib;
    }
}