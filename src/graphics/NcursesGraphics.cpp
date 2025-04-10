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
#include <ncurses.h>

namespace arcd {

namespace AnsiColors {
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string BRIGHT_BLACK = "\033[90m";
    const std::string BRIGHT_RED = "\033[91m";
    const std::string BRIGHT_GREEN = "\033[92m";
    const std::string BRIGHT_YELLOW = "\033[93m";
    const std::string BRIGHT_BLUE = "\033[94m";
    const std::string BRIGHT_MAGENTA = "\033[95m";
    const std::string BRIGHT_CYAN = "\033[96m";
    const std::string BRIGHT_WHITE = "\033[97m";
}

// Box drawing characters
namespace BoxChars {
    const std::string TOP_LEFT = "┌";
    const std::string TOP_RIGHT = "┐";
    const std::string BOTTOM_LEFT = "└";
    const std::string BOTTOM_RIGHT = "┘";
    const std::string HORIZONTAL = "─";
    const std::string VERTICAL = "│";
}

NcursesGraphics::NcursesGraphics() : 
    _initialized(false), 
    _width(80), 
    _height(24),
    _lastKey(0),
    _frameCounter(0)
{
}

NcursesGraphics::~NcursesGraphics()
{
    cleanup();
}

void NcursesGraphics::updateTerminalSize() {
    if (!_initialized) return;
    getmaxyx(stdscr, _height, _width);
}

bool NcursesGraphics::initialize()
{
    if (_initialized) return true;
    
    // Initialize ncurses with proper settings
    setlocale(LC_ALL, "");  // Enable UTF-8
    initscr();              // Initialize ncurses
    raw();                  // Disable line buffering
    noecho();              // Don't echo keypresses
    curs_set(0);           // Hide cursor
    keypad(stdscr, TRUE);  // Enable keypad input
    nodelay(stdscr, TRUE); // Non-blocking input
    start_color();         // Enable colors
    use_default_colors();  // Use terminal's default colors
    
    // Initialize color pairs
    init_pair(1, COLOR_BLACK, -1);
    init_pair(2, COLOR_RED, -1);
    init_pair(3, COLOR_GREEN, -1);
    init_pair(4, COLOR_YELLOW, -1);
    init_pair(5, COLOR_BLUE, -1);
    init_pair(6, COLOR_MAGENTA, -1);
    init_pair(7, COLOR_CYAN, -1);
    init_pair(8, COLOR_WHITE, -1);
    
    // Get terminal size
    updateTerminalSize();
    
    _initialized = true;
    return true;
}

void NcursesGraphics::cleanup()
{
    if (!_initialized) return;
    
    // Clear the screen and refresh one last time
    clear();
    refresh();
    
    // Disable ncurses mode
    keypad(stdscr, FALSE);
    nocbreak();
    echo();
    
    // End ncurses
    endwin();
    
    // Reset terminal state
    curs_set(1);  // Show cursor
    
    _initialized = false;
    _width = 80;
    _height = 24;
    _lastKey = 0;
    _frameCounter = 0;
    _colorPairs.clear();
}

void NcursesGraphics::clear()
{
    if (!_initialized) return;
    erase();
}

void NcursesGraphics::refresh()
{
    if (!_initialized) return;
    updateTerminalSize();
    ::refresh();
}

void NcursesGraphics::drawText(int x, int y, const std::string& text, Color color)
{
    if (!_initialized) return;
    attron(COLOR_PAIR(static_cast<int>(color)));
    mvaddstr(y, x, text.c_str());
    attroff(COLOR_PAIR(static_cast<int>(color)));
}

void NcursesGraphics::drawTextCentered(int y, const std::string& text, Color color) {
    if (!_initialized) return;
    int x = (_width - text.length()) / 2;
    drawText(x, y, text, color);
}

void NcursesGraphics::drawBox(int x, int y, int width, int height, Color color)
{
    if (!_initialized) return;
    
    attron(COLOR_PAIR(static_cast<int>(color)));
    
    // Box corners
    mvaddstr(y, x, "┌");
    mvaddstr(y, x + width - 1, "┐");
    mvaddstr(y + height - 1, x, "└");
    mvaddstr(y + height - 1, x + width - 1, "┘");
    
    // Horizontal lines
    for (int i = x + 1; i < x + width - 1; i++) {
        mvaddstr(y, i, "─");
        mvaddstr(y + height - 1, i, "─");
    }
    
    // Vertical lines
    for (int i = y + 1; i < y + height - 1; i++) {
        mvaddstr(i, x, "│");
        mvaddstr(i, x + width - 1, "│");
    }
    
    attroff(COLOR_PAIR(static_cast<int>(color)));
}

void NcursesGraphics::drawBoxWithTitle(int x, int y, int width, int height, const std::string& title, Color color) {
    if (!_initialized) return;
    drawBox(x, y, width, height, color);
    int titleX = x + (width - title.length()) / 2;
    drawText(titleX, y, title, color);
}

void NcursesGraphics::drawFilledBox(int x, int y, int width, int height, char fillChar, Color color) {
    if (!_initialized) return;
    attron(COLOR_PAIR(static_cast<int>(color)));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            mvprintw(y + i, x + j, "%c", fillChar);
        }
    }
    attroff(COLOR_PAIR(static_cast<int>(color)));
}

void NcursesGraphics::drawHorizontalLine(int x, int y, int length, Color color) {
    if (!_initialized) return;
    attron(COLOR_PAIR(static_cast<int>(color)));
    for (int i = 0; i < length; i++) {
        mvprintw(y, x + i, "─");
    }
    attroff(COLOR_PAIR(static_cast<int>(color)));
}

void NcursesGraphics::drawVerticalLine(int x, int y, int height, Color color) {
    if (!_initialized) return;
    attron(COLOR_PAIR(static_cast<int>(color)));
    for (int i = 0; i < height; i++) {
        mvprintw(y + i, x, "│");
    }
    attroff(COLOR_PAIR(static_cast<int>(color)));
}

void NcursesGraphics::drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color)
{
    if (!_initialized) return;
    
    for (size_t i = 0; i < items.size(); i++) {
        if (static_cast<int>(i) == selectedIndex) {
            attron(COLOR_PAIR(static_cast<int>(color)) | A_BOLD);
            mvaddstr(y + i, x, "> ");
            mvaddstr(y + i, x + 2, items[i].c_str());
            attroff(COLOR_PAIR(static_cast<int>(color)) | A_BOLD);
        } else {
            attron(COLOR_PAIR(static_cast<int>(color)));
            mvaddstr(y + i, x, "  ");
            mvaddstr(y + i, x + 2, items[i].c_str());
            attroff(COLOR_PAIR(static_cast<int>(color)));
        }
    }
}

void NcursesGraphics::getPlayerName(std::string& playerName)
{
    if (!_initialized) return;
    
    clear();
    
    // Draw title box
    int boxWidth = 40;
    int boxHeight = 3;
    int startX = (_width - boxWidth) / 2;
    int startY = (_height - boxHeight) / 2 - 2;
    
    // Draw header
    drawBoldText(startX + (boxWidth - 16) / 2, startY - 2, "Enter Your Name:", Color::CYAN);
    
    // Draw input box
    drawBox(startX, startY, boxWidth, boxHeight, Color::WHITE);
    
    // Enable input mode
    curs_set(1);  // Show cursor
    echo();       // Show typed characters
    
    // Move cursor to input position
    int inputX = startX + 2;
    int inputY = startY + 1;
    move(inputY, inputX);
    refresh();
    
    // Get input
    char input[256];
    memset(input, 0, sizeof(input));
    
    // Configure input field
    WINDOW* inputWin = newwin(1, boxWidth - 4, inputY, inputX);
    keypad(inputWin, TRUE);
    
    // Read input
    int ch;
    int pos = 0;
    while ((ch = wgetch(inputWin)) != '\n' && ch != KEY_ENTER && ch != KEY_ESC_CODE) {
        if (ch == KEY_BACKSPACE || ch == 127) {
            if (pos > 0) {
                pos--;
                input[pos] = '\0';
                mvwaddch(inputWin, 0, pos, ' ');
                wmove(inputWin, 0, pos);
                wrefresh(inputWin);
            }
        } else if (pos < static_cast<int>(sizeof(input) - 1) && isprint(ch)) {
            input[pos] = ch;
            pos++;
            input[pos] = '\0';
        }
        wrefresh(inputWin);
    }
    
    // Restore terminal settings
    noecho();
    curs_set(0);
    
    // Update player name if input is not empty
    if (pos > 0) {
        playerName = input;
    }
    
    // Clean up
    delwin(inputWin);
    clear();
    refresh();
}

int NcursesGraphics::getKey()
{
    if (!_initialized) return 0;
    
    int ch = getch();
    if (ch == ERR) return 0;
    
    switch (ch) {
        case KEY_UP: return KEY_UP_CODE;
        case KEY_DOWN: return KEY_DOWN_CODE;
        case KEY_LEFT: return KEY_LEFT_CODE;
        case KEY_RIGHT: return KEY_RIGHT_CODE;
        case 27: return KEY_ESC_CODE;
        case KEY_BACKSPACE: return KEY_BACKSPACE_CODE;
        case 10: return KEY_ENTER_CODE;
        case '9': return KEY_NEXT_LIB_CODE;
        case '7': return KEY_NEXT_GAME_CODE;
        default: return ch;
    }
}

void NcursesGraphics::flushInputBuffer()
{
    if (!_initialized) return;
    flushinp();  // Use ncurses flushinp instead of tcflush
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
    
    // Draw title box
    int titleWidth = 40;
    int titleHeight = 3;
    int startX = (_width - titleWidth) / 2;
    int startY = (_height - titleHeight) / 2;
    
    drawBox(startX, startY, titleWidth, titleHeight, Color::CYAN);
    drawBoldText(startX + (titleWidth - 6) / 2, startY + 1, "ARCADE", Color::CYAN);
    
    refresh();
    napms(2000);
    clear();
    refresh();
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
void NcursesGraphics::drawProgressBar(int x, int y, int width, int value, int maxValue, Color color) {
    if (!_initialized) return;
    int progress = static_cast<int>((static_cast<float>(value) / maxValue) * (width - 2));
    attron(COLOR_PAIR(static_cast<int>(color)));
    mvprintw(y, x, "[");
    for (int i = 0; i < progress; i++) {
        mvprintw(y, x + 1 + i, "=");
    }
    for (int i = progress; i < width - 2; i++) {
        mvprintw(y, x + 1 + i, " ");
    }
    mvprintw(y, x + width - 1, "]");
    attroff(COLOR_PAIR(static_cast<int>(color)));
}

// ColorMapper implementation
std::string ColorMapper::getAnsiCode(Color color) {
    switch (color) {
        case Color::BLACK: return "\033[30m";
        case Color::RED: return "\033[31m";
        case Color::GREEN: return "\033[32m";
        case Color::YELLOW: return "\033[33m";
        case Color::BLUE: return "\033[34m";
        case Color::MAGENTA: return "\033[35m";
        case Color::CYAN: return "\033[36m";
        case Color::WHITE: return "\033[37m";
        default: return AnsiColors::RESET;
    }
}

std::string ColorMapper::getBrightAnsiCode(Color color) {
    switch (color) {
        case Color::BLACK: return AnsiColors::BRIGHT_BLACK;
        case Color::RED: return AnsiColors::BRIGHT_RED;
        case Color::GREEN: return AnsiColors::BRIGHT_GREEN;
        case Color::YELLOW: return AnsiColors::BRIGHT_YELLOW;
        case Color::BLUE: return AnsiColors::BRIGHT_BLUE;
        case Color::MAGENTA: return AnsiColors::BRIGHT_MAGENTA;
        case Color::CYAN: return AnsiColors::BRIGHT_CYAN;
        case Color::WHITE: return AnsiColors::BRIGHT_WHITE;
        default: return AnsiColors::RESET;
    }
}

std::string ColorMapper::getBold() {
    return AnsiColors::BOLD;
}

std::string ColorMapper::getReset() {
    return AnsiColors::RESET;
}

void NcursesGraphics::drawBoldText(int x, int y, const std::string& text, Color color)
{
    if (!_initialized) return;
    attron(COLOR_PAIR(static_cast<int>(color)) | A_BOLD);
    mvaddstr(y, x, text.c_str());
    attroff(COLOR_PAIR(static_cast<int>(color)) | A_BOLD);
}

} // namespace arcd

extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary() {
        return std::make_unique<arcd::NcursesGraphics>();
    }
    
    void destroyGraphicsLibrary([[maybe_unused]] arcd::IGraphicsLibrary* graphicsLib) {
        // With smart pointers, this function is not needed anymore
        // but we keep it for compatibility
    }
}