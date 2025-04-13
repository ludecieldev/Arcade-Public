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
#include <filesystem>

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

// Implémentation des méthodes d'encapsulation
void NcursesGraphics::ncursesInit() {
    initscr();
}

void NcursesGraphics::ncursesEnd() {
    endwin();
}

void NcursesGraphics::ncursesRaw() {
    raw();
}

void NcursesGraphics::ncursesNoEcho() {
    noecho();
}

void NcursesGraphics::ncursesEcho() {
    echo();
}

void NcursesGraphics::ncursesSetCursor(int visibility) {
    curs_set(visibility);
}

void NcursesGraphics::ncursesKeypad(WINDOW* window, bool enable) {
    keypad(window, enable ? TRUE : FALSE);
}

void NcursesGraphics::ncursesNodelay(WINDOW* window, bool enable) {
    nodelay(window, enable ? TRUE : FALSE);
}

void NcursesGraphics::ncursesStartColor() {
    start_color();
}

void NcursesGraphics::ncursesUseDefaultColors() {
    use_default_colors();
}

void NcursesGraphics::ncursesInitPair(int pairIndex, int foreground, int background) {
    init_pair(pairIndex, foreground, background);
}

void NcursesGraphics::ncursesGetMaxYX(WINDOW* win, int& height, int& width) {
    getmaxyx(win, height, width);
}

void NcursesGraphics::ncursesAttron(int attrs) {
    attron(attrs);
}

void NcursesGraphics::ncursesAttroff(int attrs) {
    attroff(attrs);
}

void NcursesGraphics::ncursesAddstr(int y, int x, const std::string& str) {
    mvaddstr(y, x, str.c_str());
}

void NcursesGraphics::ncursesMove(int y, int x) {
    move(y, x);
}

int NcursesGraphics::ncursesGetch() {
    return getch();
}

void NcursesGraphics::ncursesTimeout(int delay) {
    timeout(delay);
}

void NcursesGraphics::ncursesFlushInput() {
    flushinp();
}

void NcursesGraphics::ncursesErase() {
    erase();
}

/**
 * @brief Update the terminal dimensions
 * - Get terminal size from ncurses
 */
void NcursesGraphics::updateTerminalSize() {
    if (!_initialized) return;
    ncursesGetMaxYX(stdscr, _height, _width);
}

/**
 * @brief Initialize the graphics library
 * - Check for required font file existence
 * - Initialize ncurses with proper settings
 * - Configure color pairs
 * - Get terminal size
 * 
 * @return true if initialization succeeded, false otherwise
 */
bool NcursesGraphics::initialize()
{
    if (_initialized) return true;
    
    std::string fontPath = "assets/fonts/Arial.ttf";
    if (!std::filesystem::exists(fontPath)) {
        std::cerr << "Error: Font file not found: " << fontPath << std::endl;
        std::cerr << "Ncurses Graphics library cannot start without this font." << std::endl;
        exit(84);
    }
    
    setlocale(LC_ALL, "");
    ncursesInit();
    ncursesRaw();
    ncursesNoEcho();
    ncursesSetCursor(0);
    ncursesKeypad(stdscr, true);
    ncursesNodelay(stdscr, true);
    ncursesStartColor();
    ncursesUseDefaultColors();
    
    ncursesInitPair(1, COLOR_BLACK, -1);
    ncursesInitPair(2, COLOR_RED, -1);
    ncursesInitPair(3, COLOR_GREEN, -1);
    ncursesInitPair(4, COLOR_YELLOW, -1);
    ncursesInitPair(5, COLOR_BLUE, -1);
    ncursesInitPair(6, COLOR_MAGENTA, -1);
    ncursesInitPair(7, COLOR_CYAN, -1);
    ncursesInitPair(8, COLOR_WHITE, -1);
    
    updateTerminalSize();
    
    _initialized = true;
    return true;
}

/**
 * @brief Clean up resources used by the graphics library
 * - Clear and refresh the screen one last time
 * - Disable ncurses mode
 * - End ncurses
 * - Reset terminal state
 * - Reset class variables
 */
void NcursesGraphics::cleanup()
{
    if (!_initialized) return;
    
    clear();
    refresh();
    
    ncursesKeypad(stdscr, false);
    nocbreak();
    ncursesEcho();
    
    ncursesEnd();
    
    ncursesSetCursor(1);
    
    _initialized = false;
    _width = 80;
    _height = 24;
    _lastKey = 0;
    _frameCounter = 0;
    _colorPairs.clear();
}

/**
 * @brief Clear the screen
 */
void NcursesGraphics::clear()
{
    if (!_initialized) return;
    ncursesErase();
}

/**
 * @brief Refresh the display
 * - Update terminal size
 * - Call ncurses refresh function
 */
void NcursesGraphics::refresh()
{
    if (!_initialized) return;
    updateTerminalSize();
    ::refresh();
}

/**
 * @brief Draw text at the specified position
 * - Apply color settings
 * - Draw text at specified position
 * - Reset color settings
 * 
 * @param x x-coordinate
 * @param y y-coordinate
 * @param text the text to draw
 * @param color color to use for drawing
 */
void NcursesGraphics::drawText(int x, int y, const std::string& text, Color color)
{
    if (!_initialized) return;
    ncursesAttron(COLOR_PAIR(static_cast<int>(color)));
    ncursesAddstr(y, x, text);
    ncursesAttroff(COLOR_PAIR(static_cast<int>(color)));
}

/**
 * @brief Draw text centered horizontally at the specified y position
 * - Calculate x position based on screen width and text length
 * - Draw text at calculated position
 * 
 * @param y y-coordinate
 * @param text the text to draw
 * @param color color to use for drawing
 */
void NcursesGraphics::drawTextCentered(int y, const std::string& text, Color color) {
    if (!_initialized) return;
    int x = (_width - text.length()) / 2;
    drawText(x, y, text, color);
}

/**
 * @brief Draw a box at the specified position
 * - Apply color settings
 * - Draw box corners
 * - Draw horizontal lines
 * - Draw vertical lines
 * - Reset color settings
 * 
 * @param x x-coordinate
 * @param y y-coordinate
 * @param width width of the box
 * @param height height of the box
 * @param color color to use for drawing
 */
void NcursesGraphics::drawBox(int x, int y, int width, int height, Color color)
{
    if (!_initialized) return;
    
    ncursesAttron(COLOR_PAIR(static_cast<int>(color)));
    
    ncursesAddstr(y, x, "┌");
    ncursesAddstr(y, x + width - 1, "┐");
    ncursesAddstr(y + height - 1, x, "└");
    ncursesAddstr(y + height - 1, x + width - 1, "┘");
    
    for (int i = x + 1; i < x + width - 1; i++) {
        ncursesAddstr(y, i, "─");
        ncursesAddstr(y + height - 1, i, "─");
    }
    
    for (int i = y + 1; i < y + height - 1; i++) {
        ncursesAddstr(i, x, "│");
        ncursesAddstr(i, x + width - 1, "│");
    }
    
    ncursesAttroff(COLOR_PAIR(static_cast<int>(color)));
}

/**
 * @brief Draw a box with a title
 * - Draw the box
 * - Calculate title position
 * - Draw the title
 * 
 * @param x x-coordinate
 * @param y y-coordinate
 * @param width width of the box
 * @param height height of the box
 * @param title title text for the box
 * @param color color to use for drawing
 */
void NcursesGraphics::drawBoxWithTitle(int x, int y, int width, int height, const std::string& title, Color color) {
    if (!_initialized) return;
    drawBox(x, y, width, height, color);
    int titleX = x + (width - title.length()) / 2;
    drawText(titleX, y, title, color);
}

/**
 * @brief Draw a filled box
 * - Apply color settings
 * - Fill the box with the specified character
 * - Reset color settings
 * 
 * @param x x-coordinate
 * @param y y-coordinate
 * @param width width of the box
 * @param height height of the box
 * @param fillChar character to fill the box with
 * @param color color to use for drawing
 */
void NcursesGraphics::drawFilledBox(int x, int y, int width, int height, char fillChar, Color color) {
    if (!_initialized) return;
    ncursesAttron(COLOR_PAIR(static_cast<int>(color)));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            ncursesPrintw(y + i, x + j, "%c", fillChar);
        }
    }
    ncursesAttroff(COLOR_PAIR(static_cast<int>(color)));
}

/**
 * @brief Draw a horizontal line
 * - Apply color settings
 * - Draw horizontal line characters
 * - Reset color settings
 * 
 * @param x x-coordinate of start
 * @param y y-coordinate
 * @param length length of the line
 * @param color color to use for drawing
 */
void NcursesGraphics::drawHorizontalLine(int x, int y, int length, Color color) {
    if (!_initialized) return;
    ncursesAttron(COLOR_PAIR(static_cast<int>(color)));
    for (int i = 0; i < length; i++) {
        ncursesPrintw(y, x + i, "─");
    }
    ncursesAttroff(COLOR_PAIR(static_cast<int>(color)));
}

/**
 * @brief Draw a vertical line
 * - Apply color settings
 * - Draw vertical line characters
 * - Reset color settings
 * 
 * @param x x-coordinate
 * @param y y-coordinate of start
 * @param height height of the line
 * @param color color to use for drawing
 */
void NcursesGraphics::drawVerticalLine(int x, int y, int height, Color color) {
    if (!_initialized) return;
    ncursesAttron(COLOR_PAIR(static_cast<int>(color)));
    for (int i = 0; i < height; i++) {
        ncursesPrintw(y + i, x, "│");
    }
    ncursesAttroff(COLOR_PAIR(static_cast<int>(color)));
}

/**
 * @brief Draw a list of items with a selected index
 * - Apply color settings based on selection
 * - Draw each item with appropriate formatting
 * - Reset color settings
 * 
 * @param x x-coordinate
 * @param y y-coordinate
 * @param items vector of strings to display
 * @param selectedIndex index of the selected item
 * @param color color to use for drawing
 */
void NcursesGraphics::drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color)
{
    if (!_initialized) return;
    
    for (size_t i = 0; i < items.size(); i++) {
        if (static_cast<int>(i) == selectedIndex) {
            ncursesAttron(COLOR_PAIR(static_cast<int>(color)) | A_BOLD);
            ncursesAddstr(y + i, x, "> ");
            ncursesAddstr(y + i, x + 2, items[i]);
            ncursesAttroff(COLOR_PAIR(static_cast<int>(color)) | A_BOLD);
        } else {
            ncursesAttron(COLOR_PAIR(static_cast<int>(color)));
            ncursesAddstr(y + i, x, "  ");
            ncursesAddstr(y + i, x + 2, items[i]);
            ncursesAttroff(COLOR_PAIR(static_cast<int>(color)));
        }
    }
}

/**
 * @brief Get the player's name via user input
 * - Clear the screen
 * - Draw title and input box
 * - Configure input mode
 * - Read and process user input
 * - Restore terminal settings
 * - Update player name if input is not empty
 * - Clean up
 * 
 * @param playerName reference to string to store the player name
 */
void NcursesGraphics::getPlayerName(std::string& playerName)
{
    if (!_initialized) return;
    
    clear();
    
    int boxWidth = 40;
    int boxHeight = 3;
    int startX = (_width - boxWidth) / 2;
    int startY = (_height - boxHeight) / 2 - 2;
    
    drawBoldText(startX + (boxWidth - 16) / 2, startY - 2, "Enter Your Name:", Color::CYAN);
    
    drawBox(startX, startY, boxWidth, boxHeight, Color::WHITE);
    
    ncursesSetCursor(1);
    ncursesEcho();
    
    int inputX = startX + 2;
    int inputY = startY + 1;
    ncursesMove(inputY, inputX);
    refresh();
    
    char input[256];
    memset(input, 0, sizeof(input));
    
    // Création d'une fenêtre Ncurses avec WindowPtr
    WindowPtr inputWin(newwin(1, boxWidth - 4, inputY, inputX));
    if (!inputWin) {
        ncursesNoEcho();
        ncursesSetCursor(0);
        return;
    }
    
    ncursesKeypad(inputWin.get(), true);
    
    int ch;
    int pos = 0;
    while ((ch = wgetch(inputWin.get())) != '\n' && ch != KEY_ENTER && ch != KEY_ESC_CODE) {
        if (ch == KEY_BACKSPACE || ch == 127) {
            if (pos > 0) {
                pos--;
                input[pos] = '\0';
                mvwaddch(inputWin.get(), 0, pos, ' ');
                wmove(inputWin.get(), 0, pos);
                wrefresh(inputWin.get());
            }
        } else if (pos < static_cast<int>(sizeof(input) - 1) && isprint(ch)) {
            input[pos] = ch;
            pos++;
            input[pos] = '\0';
        }
        wrefresh(inputWin.get());
    }
    
    ncursesNoEcho();
    ncursesSetCursor(0);
    
    if (pos > 0) {
        playerName = input;
    }
    
    // Le destructeur du WindowPtr va automatiquement appeler delwin
    clear();
    refresh();
}

/**
 * @brief Get the last key pressed
 * - Check if library is initialized
 * - Get key from ncurses
 * - Map ncurses key codes to application key codes
 * 
 * @return integer code of the key or 0 if no key pressed
 */
int NcursesGraphics::getKey()
{
    if (!_initialized) return 0;
    
    int ch = ncursesGetch();
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
        case 'r': return KEY_RESTART_GAME;
        case 'R': return KEY_RESTART_GAME;
        default: return ch;
    }
}

/**
 * @brief Flush the input buffer
 * - Clear any pending input
 */
void NcursesGraphics::flushInputBuffer()
{
    if (!_initialized) return;
    ncursesFlushInput();
}

/**
 * @brief Initialize color pairs for ncurses
 * - Use default terminal colors
 * - Initialize color pairs for each Color enum value
 */
void NcursesGraphics::initColors()
{
    if (!_initialized) return;
    
    ncursesUseDefaultColors();
    
    ncursesInitPair(static_cast<int>(Color::DEFAULT), -1, -1);
    ncursesInitPair(static_cast<int>(Color::BLACK), COLOR_BLACK, -1);
    ncursesInitPair(static_cast<int>(Color::RED), COLOR_RED, -1);
    ncursesInitPair(static_cast<int>(Color::GREEN), COLOR_GREEN, -1);
    ncursesInitPair(static_cast<int>(Color::YELLOW), COLOR_YELLOW, -1);
    ncursesInitPair(static_cast<int>(Color::BLUE), COLOR_BLUE, -1);
    ncursesInitPair(static_cast<int>(Color::MAGENTA), COLOR_MAGENTA, -1);
    ncursesInitPair(static_cast<int>(Color::CYAN), COLOR_CYAN, -1);
    ncursesInitPair(static_cast<int>(Color::WHITE), COLOR_WHITE, -1);
}

/**
 * @brief Get the color pair index for foreground and background colors
 * 
 * @param fg foreground color
 * @param bg background color (unused in this implementation)
 * @return color pair index
 */
int NcursesGraphics::getColorPair(int fg, [[maybe_unused]] int bg)
{
    return fg;
}

/**
 * @brief Get the name of the graphics library
 * 
 * @return name of the graphics library
 */
std::string NcursesGraphics::getName() const
{
    return "Ncurses";
}

/**
 * @brief Show a splash screen
 * - Draw a frame
 * - Draw title and welcome message
 * - Display instructions
 * - Wait for a key press or timeout
 * - Clear screen before continuing
 */
void NcursesGraphics::showSplashScreen()
{
    if (!_initialized) return;
    
    clear();
    
    drawBox(2, 1, _width - 4, _height - 2, Color::CYAN);
    
    std::string title = "ARCADE";
    drawTextCentered(3, title, Color::YELLOW);
    
    std::string welcome = "Welcome to the Arcade Game Platform";
    drawTextCentered(5, welcome, Color::WHITE);
    
    std::string instruction = "Press any key to continue...";
    drawTextCentered(_height - 5, instruction, Color::WHITE);
    
    refresh();
    
    ncursesTimeout(2000);
    ncursesGetch();
    ncursesTimeout(10);
    
    clear();
    refresh();
}

/**
 * @brief Wait for a key press with timeout
 * - Set up polling for input
 * - Wait for specified timeout
 * - Return key code if input is available
 * 
 * @param timeoutMs timeout in milliseconds
 * @return key code or 0 if timeout
 */
int NcursesGraphics::waitForKey(int timeoutMs)
{
    struct pollfd pfd = {STDIN_FILENO, POLLIN, 0};
    int result = poll(&pfd, 1, timeoutMs);
    
    if (result > 0 && (pfd.revents & POLLIN)) {
        return getKey();
    }
    
    return 0;
}

/**
 * @brief Wait for any key press
 * - Flush input buffer
 * - Poll for input with short timeout
 * - Continue until a key is pressed
 * - Flush input buffer again
 */
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

/**
 * @brief Draw a progress bar
 * - Calculate progress based on value and maximum
 * - Draw progress bar frame
 * - Fill bar proportionally to progress
 * 
 * @param x x-coordinate
 * @param y y-coordinate
 * @param width width of the progress bar
 * @param value current value
 * @param maxValue maximum value
 * @param color color to use for drawing
 */
void NcursesGraphics::drawProgressBar(int x, int y, int width, int value, int maxValue, Color color) {
    if (!_initialized) return;
    int progress = static_cast<int>((static_cast<float>(value) / maxValue) * (width - 2));
    ncursesAttron(COLOR_PAIR(static_cast<int>(color)));
    ncursesPrintw(y, x, "[");
    for (int i = 0; i < progress; i++) {
        ncursesPrintw(y, x + 1 + i, "=");
    }
    for (int i = progress; i < width - 2; i++) {
        ncursesPrintw(y, x + 1 + i, " ");
    }
    ncursesPrintw(y, x + width - 1, "]");
    ncursesAttroff(COLOR_PAIR(static_cast<int>(color)));
}

/**
 * @brief Get ANSI color code for a Color enum value
 * 
 * @param color Color enum value
 * @return ANSI color code string
 */
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

/**
 * @brief Get bright ANSI color code for a Color enum value
 * 
 * @param color Color enum value
 * @return Bright ANSI color code string
 */
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

/**
 * @brief Get ANSI bold code
 * 
 * @return ANSI bold code string
 */
std::string ColorMapper::getBold() {
    return AnsiColors::BOLD;
}

/**
 * @brief Get ANSI reset code
 * 
 * @return ANSI reset code string
 */
std::string ColorMapper::getReset() {
    return AnsiColors::RESET;
}

/**
 * @brief Draw bold text at the specified position
 * - Apply color and bold settings
 * - Draw text at specified position
 * - Reset settings
 * 
 * @param x x-coordinate
 * @param y y-coordinate
 * @param text the text to draw
 * @param color color to use for drawing
 */
void NcursesGraphics::drawBoldText(int x, int y, const std::string& text, Color color)
{
    if (!_initialized) return;
    ncursesAttron(COLOR_PAIR(static_cast<int>(color)) | A_BOLD);
    ncursesAddstr(y, x, text);
    ncursesAttroff(COLOR_PAIR(static_cast<int>(color)) | A_BOLD);
}

/**
 * @brief Draw the menu screen
 * - Clear the screen
 * - Draw centered title with decoration
 * - Calculate positions for menu boxes
 * - Process game and graphics library names for display
 * - Draw game options box and items
 * - Draw graphics options box and items
 * - Draw player options box and items
 * - Draw instructions box with controls
 * - Refresh the display
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
void NcursesGraphics::drawMenu(
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
    
    ncursesAttron(COLOR_PAIR(getColorPair(COLOR_WHITE, COLOR_BLACK)) | A_BOLD);
    drawTextCentered(1, "=== " + title + " ===", Color::WHITE);
    ncursesAttroff(A_BOLD);
    
    int boxWidth = 25;
    int boxHeight = 12;
    int spacing = 4;
    int startX = (_width - (3 * boxWidth + 2 * spacing)) / 2;
    int startY = 3;
    
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
    
    bool isGameBoxSelected = (selectedMenu == 0);
    Color gameBoxColor = isGameBoxSelected ? Color::YELLOW : Color::WHITE;
    drawBoxWithTitle(startX, startY, boxWidth, boxHeight, "Games", gameBoxColor);
    
    for (size_t i = 0; i < displayGameNames.size() && i < static_cast<size_t>(boxHeight - 2); i++) {
        Color itemColor = (isGameBoxSelected && static_cast<int>(i) == selectedGameIndex) 
                          ? Color::YELLOW : Color::WHITE;
        
        if (isGameBoxSelected && static_cast<int>(i) == selectedGameIndex) {
            ncursesAttron(COLOR_PAIR(getColorPair(COLOR_YELLOW, COLOR_BLACK)) | A_BOLD);
            ncursesAddstr(startY + 2 + static_cast<int>(i), startX + 2, "> %s", displayGameNames[i].c_str());
            ncursesAttroff(COLOR_PAIR(getColorPair(COLOR_YELLOW, COLOR_BLACK)) | A_BOLD);
        } else {
            drawText(startX + 2, startY + 2 + static_cast<int>(i), displayGameNames[i], itemColor);
        }
    }
    
    bool isGraphicsBoxSelected = (selectedMenu == 1);
    Color graphicsBoxColor = isGraphicsBoxSelected ? Color::YELLOW : Color::WHITE;
    drawBoxWithTitle(startX + boxWidth + spacing, startY, boxWidth, boxHeight, "Graphics", graphicsBoxColor);
    
    for (size_t i = 0; i < displayGraphicNames.size() && i < static_cast<size_t>(boxHeight - 2); i++) {
        Color itemColor = (isGraphicsBoxSelected && static_cast<int>(i) == selectedGraphicIndex) 
                          ? Color::YELLOW : Color::WHITE;
        
        if (isGraphicsBoxSelected && static_cast<int>(i) == selectedGraphicIndex) {
            ncursesAttron(COLOR_PAIR(getColorPair(COLOR_YELLOW, COLOR_BLACK)) | A_BOLD);
            ncursesAddstr(startY + 2 + static_cast<int>(i), startX + boxWidth + spacing + 2, "> %s", displayGraphicNames[i].c_str());
            ncursesAttroff(COLOR_PAIR(getColorPair(COLOR_YELLOW, COLOR_BLACK)) | A_BOLD);
        } else {
            drawText(startX + boxWidth + spacing + 2, startY + 2 + static_cast<int>(i), 
                    displayGraphicNames[i], itemColor);
        }
    }
    
    bool isPlayerBoxSelected = (selectedMenu == 2);
    Color playerBoxColor = isPlayerBoxSelected ? Color::YELLOW : Color::WHITE;
    drawBoxWithTitle(startX + 2 * (boxWidth + spacing), startY, boxWidth, boxHeight, "Player: " + playerName, playerBoxColor);
    
    std::vector<std::string> playerOptions = {"Change Nickname", "Leaderboard"};
    for (size_t i = 0; i < playerOptions.size(); i++) {
        Color itemColor = (isPlayerBoxSelected && static_cast<int>(i) == playerOptionSelected) 
                          ? Color::YELLOW : Color::WHITE;
        
        if (isPlayerBoxSelected && static_cast<int>(i) == playerOptionSelected) {
            ncursesAttron(COLOR_PAIR(getColorPair(COLOR_YELLOW, COLOR_BLACK)) | A_BOLD);
            ncursesAddstr(startY + 2 + static_cast<int>(i), startX + 2 * (boxWidth + spacing) + 2, "> %s", playerOptions[i].c_str());
            ncursesAttroff(COLOR_PAIR(getColorPair(COLOR_YELLOW, COLOR_BLACK)) | A_BOLD);
        } else {
            drawText(startX + 2 * (boxWidth + spacing) + 2, startY + 2 + static_cast<int>(i), 
                    playerOptions[i], itemColor);
        }
    }
    
    int instructionY = startY + boxHeight + 2;
    drawBox(startX, instructionY, 3 * boxWidth + 2 * spacing, 9, Color::BLUE);
    
    ncursesAttron(COLOR_PAIR(getColorPair(COLOR_CYAN, COLOR_BLACK)) | A_BOLD);
    drawTextCentered(instructionY, "CONTROLS", Color::CYAN);
    ncursesAttroff(A_BOLD);
    
    std::vector<std::pair<std::string, std::string>> instructions = {
        {"SELECT", "TAB key"},
        {"NAVIGATE", "Arrow keys"},
        {"CONFIRM", "Enter key"},
        {"EXIT", "Escape key"}
    };
    
    for (size_t i = 0; i < instructions.size(); i++) {
        int y = instructionY + 2 + static_cast<int>(i) * 2;
        
        ncursesAttron(COLOR_PAIR(getColorPair(COLOR_YELLOW, COLOR_BLACK)) | A_BOLD);
        ncursesAddstr(y, startX + 4, "%s:", instructions[i].first.c_str());
        ncursesAttroff(A_BOLD);
        
        ncursesAttron(COLOR_PAIR(getColorPair(COLOR_WHITE, COLOR_BLACK)));
        ncursesAddstr(y, startX + 15, "%s", instructions[i].second.c_str());
        ncursesAttroff(COLOR_PAIR(getColorPair(COLOR_WHITE, COLOR_BLACK)));
    }
    
    refresh();
}

} // namespace arcd

extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary() {
        return std::make_unique<arcd::NcursesGraphics>();
    }
}