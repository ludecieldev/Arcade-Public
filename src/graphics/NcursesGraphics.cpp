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
#include <map>
#include <sstream>

namespace arcd {

// Define key constants for better readability
constexpr int KEY_UP = 259;
constexpr int KEY_DOWN = 258;
constexpr int KEY_LEFT = 260;
constexpr int KEY_RIGHT = 261;
constexpr int KEY_ENTER = 10;
constexpr int KEY_ESC = 27;
constexpr int KEY_BACKSPACE = 127;

// ANSI color codes
struct Color {
    static const std::string RESET;
    static const std::string BLACK;
    static const std::string RED;
    static const std::string GREEN;
    static const std::string YELLOW;
    static const std::string BLUE;
    static const std::string MAGENTA;
    static const std::string CYAN;
    static const std::string WHITE;
    static const std::string BRIGHT_BLACK;
    static const std::string BRIGHT_RED;
    static const std::string BRIGHT_GREEN;
    static const std::string BRIGHT_YELLOW;
    static const std::string BRIGHT_BLUE;
    static const std::string BRIGHT_MAGENTA;
    static const std::string BRIGHT_CYAN;
    static const std::string BRIGHT_WHITE;
    static const std::string BOLD;
    
    static std::string fg(int color);
    static std::string bg(int color);
};

const std::string Color::RESET = "\033[0m";
const std::string Color::BLACK = "\033[30m";
const std::string Color::RED = "\033[31m";
const std::string Color::GREEN = "\033[32m";
const std::string Color::YELLOW = "\033[33m";
const std::string Color::BLUE = "\033[34m";
const std::string Color::MAGENTA = "\033[35m";
const std::string Color::CYAN = "\033[36m";
const std::string Color::WHITE = "\033[37m";
const std::string Color::BRIGHT_BLACK = "\033[90m";
const std::string Color::BRIGHT_RED = "\033[91m";
const std::string Color::BRIGHT_GREEN = "\033[92m";
const std::string Color::BRIGHT_YELLOW = "\033[93m";
const std::string Color::BRIGHT_BLUE = "\033[94m";
const std::string Color::BRIGHT_MAGENTA = "\033[95m";
const std::string Color::BRIGHT_CYAN = "\033[96m";
const std::string Color::BRIGHT_WHITE = "\033[97m";
const std::string Color::BOLD = "\033[1m";

std::string Color::fg(int color) {
    return "\033[38;5;" + std::to_string(color) + "m";
}

std::string Color::bg(int color) {
    return "\033[48;5;" + std::to_string(color) + "m";
}

// Box drawing characters
struct BoxChars {
    static const std::string TOP_LEFT;
    static const std::string TOP_RIGHT;
    static const std::string BOTTOM_LEFT;
    static const std::string BOTTOM_RIGHT;
    static const std::string HORIZONTAL;
    static const std::string VERTICAL;
    static const std::string T_DOWN;
    static const std::string T_UP;
    static const std::string T_RIGHT;
    static const std::string T_LEFT;
    static const std::string CROSS;
};

const std::string BoxChars::TOP_LEFT = "╔";
const std::string BoxChars::TOP_RIGHT = "╗";
const std::string BoxChars::BOTTOM_LEFT = "╚";
const std::string BoxChars::BOTTOM_RIGHT = "╝";
const std::string BoxChars::HORIZONTAL = "═";
const std::string BoxChars::VERTICAL = "║";
const std::string BoxChars::T_DOWN = "╦";
const std::string BoxChars::T_UP = "╩";
const std::string BoxChars::T_RIGHT = "╠";
const std::string BoxChars::T_LEFT = "╣";
const std::string BoxChars::CROSS = "╬";

NcursesGraphics::NcursesGraphics() : 
    _initialized(false), 
    _width(80), 
    _height(24),
    _lastKey(0),
    _frameCounter(0)
{
    // Get terminal size
    updateTerminalSize();
    
    // Save original terminal settings
    tcgetattr(STDIN_FILENO, &_oldTermios);
}

NcursesGraphics::~NcursesGraphics()
{
    cleanup();
}

void NcursesGraphics::updateTerminalSize() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1) {
        _width = ws.ws_col;
        _height = ws.ws_row;
    }
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
    
    // Check if terminal size has changed
    updateTerminalSize();
    
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

void NcursesGraphics::drawTextCentered(int y, const std::string& text)
{
    drawText(_width/2 - text.length()/2, y, text);
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
    drawText(x, y, BoxChars::TOP_LEFT);
    for (int i = 1; i < width - 1; i++) {
        drawText(x + i, y, BoxChars::HORIZONTAL);
    }
    drawText(x + width - 1, y, BoxChars::TOP_RIGHT);
    
    // Sides
    for (int j = 1; j < height - 1; j++) {
        drawText(x, y + j, BoxChars::VERTICAL);
        drawText(x + width - 1, y + j, BoxChars::VERTICAL);
    }
    
    // Bottom border
    drawText(x, y + height - 1, BoxChars::BOTTOM_LEFT);
    for (int i = 1; i < width - 1; i++) {
        drawText(x + i, y + height - 1, BoxChars::HORIZONTAL);
    }
    drawText(x + width - 1, y + height - 1, BoxChars::BOTTOM_RIGHT);
}

void NcursesGraphics::drawBoxWithTitle(int x, int y, int width, int height, const std::string& title)
{
    drawBox(x, y, width, height);
    
    // Draw title if it fits
    if (title.length() + 4 < (size_t)width) {
        std::string titleText = " " + title + " ";
        drawText(x + (width - titleText.length()) / 2, y, titleText);
    }
}

void NcursesGraphics::drawFilledBox(int x, int y, int width, int height, char fillChar)
{
    if (!_initialized) return;
    
    // Validate dimensions and boundaries
    if (width <= 0 || height <= 0 || x < 0 || y < 0 || 
        x >= _width || y >= _height) return;
    
    // Ensure box fits in the screen
    width = std::min(width, _width - x);
    height = std::min(height, _height - y);
    
    std::string line(width, fillChar);
    for (int j = 0; j < height; j++) {
        drawText(x, y + j, line);
    }
}

void NcursesGraphics::drawHorizontalLine(int x, int y, int width)
{
    if (!_initialized) return;
    
    // Validate input
    if (y < 0 || y >= _height || x < 0 || width <= 0) return;
    
    // Ensure line fits in the screen
    width = std::min(width, _width - x);
    
    std::string line(width, '-');
    drawText(x, y, line);
}

void NcursesGraphics::drawVerticalLine(int x, int y, int height)
{
    if (!_initialized) return;
    
    // Validate input
    if (x < 0 || x >= _width || y < 0 || height <= 0) return;
    
    // Ensure line fits in the screen
    height = std::min(height, _height - y);
    
    for (int i = 0; i < height; i++) {
        drawText(x, y + i, "|");
    }
}

void NcursesGraphics::drawList([[maybe_unused]] int x, [[maybe_unused]] int y, 
                              const std::vector<std::string>& items, int selectedIndex)
{
    if (!_initialized || items.empty()) return;
    
    // Clear first
    clear();
    
    // Draw decorative frame around the entire screen
    std::cout << Color::BRIGHT_BLUE;
    drawBox(1, 1, _width - 2, _height - 2);
    std::cout << Color::RESET;
    
    // Title with decorative elements
    std::string title = "✧ ARCADE SYSTEM ✧";
    std::cout << Color::BRIGHT_CYAN << Color::BOLD;
    drawBoxWithTitle(_width/2 - 20, 3, 40, 3, title);
    
    // Display player info - we can't access private members directly
    std::string playerInfo = "PLAYER: GUEST";
    drawTextCentered(7, playerInfo);
    std::cout << Color::RESET;
    
    // Menu items with enhanced visual style
    const int boxWidth = 40;
    const int boxHeight = 3;
    const int spacing = 1;
    const int startY = 10;
    
    // Draw menu items
    for (size_t i = 0; i < items.size(); i++) {
        int itemY = startY + i * (boxHeight + spacing);
        
        if ((int)i == selectedIndex) {
            // Selected item with highlight effect
            std::cout << Color::BRIGHT_YELLOW << Color::BOLD;
            drawFilledBox(_width/2 - boxWidth/2 - 1, itemY - 1, boxWidth + 2, boxHeight + 2, ' ');
            drawBox(_width/2 - boxWidth/2, itemY, boxWidth, boxHeight);
            
            // Add selection indicator
            drawText(_width/2 - boxWidth/2 - 3, itemY + 1, "►");
            drawText(_width/2 + boxWidth/2 + 1, itemY + 1, "◄");
        } else {
            // Unselected item
            std::cout << Color::BRIGHT_WHITE;
            drawBox(_width/2 - boxWidth/2, itemY, boxWidth, boxHeight);
        }
        
        // Draw item text
        drawTextCentered(itemY + 1, items[i]);
        std::cout << Color::RESET;
    }
    
    // Game information section
    std::cout << Color::BRIGHT_GREEN;
    drawTextCentered(_height - 8, "ARCADE SYSTEM");
    std::cout << Color::RESET;
    
    // Draw controls with better styling
    std::cout << Color::BRIGHT_MAGENTA;
    drawBox(2, _height - 6, _width - 4, 4);
    std::cout << Color::RESET << Color::BOLD;
    
    drawTextCentered(_height - 5, "↑/↓: Navigate | Enter: Select | Q: Quit");
    drawTextCentered(_height - 4, "9: Switch Graphics | 7: Switch Game | P: Player Profile");
    std::cout << Color::RESET;
    
    // Draw current graphics library info
    std::string libInfo = "GRAPHICS: " + getName();
    std::cout << Color::BRIGHT_CYAN;
    drawText(3, 2, libInfo);
    std::cout << Color::RESET;
    
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
    std::cout << Color::BRIGHT_CYAN << Color::BOLD;
    drawBoxWithTitle(boxX, boxY - 4, boxWidth, 3, "ENTER PLAYER NAME");
    std::cout << Color::RESET;
    
    // Input box
    std::cout << Color::BRIGHT_WHITE;
    drawBox(boxX, boxY, boxWidth, boxHeight);
    std::cout << Color::RESET;
    
    // Instructions
    std::string instructions = "Enter your name (max 15 chars):";
    drawTextCentered(boxY + 1, instructions);
    
    // Status
    std::string status = "Press ENTER to confirm, ESC to cancel";
    drawTextCentered(boxY + boxHeight + 2, status);
    
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
        
        // Show input with blinking cursor
        std::string displayText = input;
        if (_frameCounter % 20 < 10) {
            displayText += "_";
        } else {
            displayText += " ";
        }
        drawText(inputX, inputY, displayText);
        
        refresh();
        
        // Get input
        int key = waitForKey(50); // 50ms timeout for smoother cursor blinking
        
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
        
        _frameCounter++;
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
    
    const std::vector<std::string> logo = {
        "    █████╗ ██████╗  ██████╗ █████╗ ██████╗ ███████╗    ",
        "   ██╔══██╗██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔════╝    ",
        "   ███████║██████╔╝██║     ███████║██║  ██║█████╗      ",
        "   ██╔══██║██╔══██╗██║     ██╔══██║██║  ██║██╔══╝      ",
        "   ██║  ██║██║  ██║╚██████╗██║  ██║██████╔╝███████╗    ",
        "   ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚═════╝ ╚══════╝    "
    };
    
    const int logoWidth = logo[0].length();
    const int logoHeight = logo.size();
    const int boxWidth = logoWidth + 4;
    const int boxHeight = logoHeight + 4;
    const int boxX = _width/2 - boxWidth/2;
    const int boxY = _height/2 - boxHeight/2;
    
    // Animation effect
    for (int frame = 0; frame < 5; frame++) {
        clear();
        
        // Draw animated border
        std::cout << Color::BRIGHT_CYAN;
        drawBox(boxX, boxY, boxWidth, boxHeight);
        std::cout << Color::RESET;
        
        // Draw logo with color that changes based on frame
        int colorCode = 31 + (frame % 6); // Cycle through colors
        std::cout << "\033[1;" << colorCode << "m";
        
        for (int i = 0; i < logoHeight; i++) {
            drawText(boxX + 2, boxY + 2 + i, logo[i]);
        }
        
        // Welcome message
        std::cout << Color::BRIGHT_WHITE << Color::BOLD;
        drawTextCentered(boxY + boxHeight + 2, "Welcome to the Arcade System!");
        drawTextCentered(boxY + boxHeight + 4, "Press any key to start...");
        
        // Controls
        std::cout << Color::BRIGHT_GREEN;
        std::string controls = "CONTROLS: Arrow Keys=Navigate | Enter=Select | 9=Next Lib | 7=Next Game";
        drawTextCentered(_height - 4, controls);
        
        std::string controls2 = "R=Restart | Q=Menu | E=Exit";
        drawTextCentered(_height - 3, controls2);
        
        std::cout << Color::RESET;
        refresh();
        
        // Brief pause between frames
        struct timespec ts = {0, 200000000}; // 200ms
        nanosleep(&ts, NULL);
    }
    
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

// Draw a progress bar
void NcursesGraphics::drawProgressBar(int x, int y, int width, int value, int maxValue)
{
    if (!_initialized) return;
    
    // Validate dimensions
    if (width < 3) return;
    
    // Calculate progress
    int progress = static_cast<int>((static_cast<float>(value) / maxValue) * (width - 2));
    progress = std::min(progress, width - 2);
    
    // Draw border
    drawText(x, y, "[");
    drawText(x + width - 1, y, "]");
    
    // Draw progress bar
    std::string progressBar(progress, '=');
    if (progress < width - 2) {
        progressBar += '>';
        progressBar += std::string(width - 3 - progress, ' ');
    }
    drawText(x + 1, y, progressBar);
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