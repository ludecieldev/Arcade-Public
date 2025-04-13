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

void NcursesGraphics::updateTerminalSize() {
    if (!_initialized) return;
    getmaxyx(stdscr, _height, _width);
}

bool NcursesGraphics::initialize()
{
    if (_initialized) return true;
    
    // Vérifier que le fichier de police existe (même si Ncurses n'en a pas directement besoin)
    std::string fontPath = "assets/fonts/Arial.ttf";
    if (!std::filesystem::exists(fontPath)) {
        std::cerr << "Error: Font file not found: " << fontPath << std::endl;
        std::cerr << "Ncurses Graphics library cannot start without this font." << std::endl;
        exit(84); // Quitter avec le code 84 si la police n'est pas trouvée
    }
    
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
        case 'r': return KEY_RESTART_GAME;
        case 'R': return KEY_RESTART_GAME;
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
    if (!_initialized) return;
    
    // Utiliser les couleurs par défaut du terminal
    use_default_colors();
    
    // Initialiser les paires de couleurs
    init_pair(static_cast<int>(Color::DEFAULT), -1, -1);
    init_pair(static_cast<int>(Color::BLACK), COLOR_BLACK, -1);
    init_pair(static_cast<int>(Color::RED), COLOR_RED, -1);
    init_pair(static_cast<int>(Color::GREEN), COLOR_GREEN, -1);
    init_pair(static_cast<int>(Color::YELLOW), COLOR_YELLOW, -1);
    init_pair(static_cast<int>(Color::BLUE), COLOR_BLUE, -1);
    init_pair(static_cast<int>(Color::MAGENTA), COLOR_MAGENTA, -1);
    init_pair(static_cast<int>(Color::CYAN), COLOR_CYAN, -1);
    init_pair(static_cast<int>(Color::WHITE), COLOR_WHITE, -1);
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
    
    // Dessiner le cadre pour l'écran de démarrage
    drawBox(2, 1, _width - 4, _height - 2, Color::CYAN);
    
    // Titre de l'application
    std::string title = "ARCADE";
    drawTextCentered(3, title, Color::YELLOW);
    
    // Message de bienvenue
    std::string welcome = "Welcome to the Arcade Game Platform";
    drawTextCentered(5, welcome, Color::WHITE);
    
    // Message d'initialisation de la souris
    std::string mouseMsg = "Mouse support initialized";
    drawTextCentered(7, mouseMsg, Color::GREEN);
    
    // Instruction
    std::string instruction = "Press any key to continue...";
    drawTextCentered(_height - 5, instruction, Color::WHITE);
    
    refresh();
    
    // Attendre une touche (avec timeout)
    timeout(2000); // Timeout de 2 secondes
    getch();       // On ignore la valeur de retour
    timeout(10);   // Retour au timeout normal
    
    // Effacer l'écran avant de continuer
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

void NcursesGraphics::drawMenu(
    const std::string& title,
    const std::vector<std::string>& gameOptions,
    const std::vector<std::string>& graphicOptions,
    const std::string& playerName,
    int selectedMenu,
    int selectedGameIndex,
    int selectedGraphicIndex)
{
    if (!_initialized) return;
    
    clear();
    
    // Draw centered title with decoration
    attron(COLOR_PAIR(getColorPair(COLOR_WHITE, COLOR_BLACK)) | A_BOLD);
    drawTextCentered(1, "=== " + title + " ===", Color::WHITE);
    attroff(A_BOLD);
    
    // Calculate positions for menu boxes
    int boxWidth = 25;  // Augmenter la largeur
    int boxHeight = 12; // Augmenter la hauteur
    int spacing = 4;
    int startX = (_width - (3 * boxWidth + 2 * spacing)) / 2;
    int startY = 3;
    
    // Préparer les noms simplifiés
    std::vector<std::string> displayGameNames;
    for (const auto& name : gameOptions) {
        std::string displayName = name;
        // Enlever le préfixe "arcade_" s'il existe
        size_t prefixPos = displayName.find("arcade_");
        if (prefixPos != std::string::npos) {
            displayName = displayName.substr(prefixPos + 7); // 7 est la longueur de "arcade_"
        }
        // Enlever l'extension ".so" s'il existe
        size_t extPos = displayName.find(".so");
        if (extPos != std::string::npos) {
            displayName = displayName.substr(0, extPos);
        }
        displayGameNames.push_back(displayName);
    }
    
    std::vector<std::string> displayGraphicNames;
    for (const auto& name : graphicOptions) {
        std::string displayName = name;
        // Enlever le préfixe "arcade_" s'il existe
        size_t prefixPos = displayName.find("arcade_");
        if (prefixPos != std::string::npos) {
            displayName = displayName.substr(prefixPos + 7); // 7 est la longueur de "arcade_"
        }
        // Enlever l'extension ".so" s'il existe
        size_t extPos = displayName.find(".so");
        if (extPos != std::string::npos) {
            displayName = displayName.substr(0, extPos);
        }
        displayGraphicNames.push_back(displayName);
    }
    
    // Draw game options box
    bool isGameBoxSelected = (selectedMenu == 0);
    Color gameBoxColor = isGameBoxSelected ? Color::YELLOW : Color::WHITE;
    drawBoxWithTitle(startX, startY, boxWidth, boxHeight, "Games", gameBoxColor);
    
    // Draw game options
    for (size_t i = 0; i < displayGameNames.size() && i < static_cast<size_t>(boxHeight - 2); i++) {
        Color itemColor = (isGameBoxSelected && static_cast<int>(i) == selectedGameIndex) 
                          ? Color::YELLOW : Color::WHITE;
        
        if (isGameBoxSelected && static_cast<int>(i) == selectedGameIndex) {
            attron(COLOR_PAIR(getColorPair(COLOR_YELLOW, COLOR_BLACK)) | A_BOLD);
            mvprintw(startY + 2 + static_cast<int>(i), startX + 2, "> %s", displayGameNames[i].c_str());
            attroff(COLOR_PAIR(getColorPair(COLOR_YELLOW, COLOR_BLACK)) | A_BOLD);
        } else {
            drawText(startX + 2, startY + 2 + static_cast<int>(i), displayGameNames[i], itemColor);
        }
    }
    
    // Draw graphics options box
    bool isGraphicsBoxSelected = (selectedMenu == 1);
    Color graphicsBoxColor = isGraphicsBoxSelected ? Color::YELLOW : Color::WHITE;
    drawBoxWithTitle(startX + boxWidth + spacing, startY, boxWidth, boxHeight, "Graphics", graphicsBoxColor);
    
    // Draw graphics options
    for (size_t i = 0; i < displayGraphicNames.size() && i < static_cast<size_t>(boxHeight - 2); i++) {
        Color itemColor = (isGraphicsBoxSelected && static_cast<int>(i) == selectedGraphicIndex) 
                          ? Color::YELLOW : Color::WHITE;
        
        if (isGraphicsBoxSelected && static_cast<int>(i) == selectedGraphicIndex) {
            attron(COLOR_PAIR(getColorPair(COLOR_YELLOW, COLOR_BLACK)) | A_BOLD);
            mvprintw(startY + 2 + static_cast<int>(i), startX + boxWidth + spacing + 2, "> %s", displayGraphicNames[i].c_str());
            attroff(COLOR_PAIR(getColorPair(COLOR_YELLOW, COLOR_BLACK)) | A_BOLD);
        } else {
            drawText(startX + boxWidth + spacing + 2, startY + 2 + static_cast<int>(i), 
                    displayGraphicNames[i], itemColor);
        }
    }
    
    // Draw player name box
    bool isPlayerBoxSelected = (selectedMenu == 2);
    Color playerBoxColor = isPlayerBoxSelected ? Color::YELLOW : Color::WHITE;
    drawBoxWithTitle(startX + 2 * (boxWidth + spacing), startY, boxWidth, boxHeight, "Player", playerBoxColor);
    
    // Display player name (shortened if too long)
    std::string displayName = playerName;
    if (displayName.length() > static_cast<size_t>(boxWidth - 4)) {
        displayName = displayName.substr(0, boxWidth - 7) + "...";
    }
    
    if (isPlayerBoxSelected) {
        attron(COLOR_PAIR(getColorPair(COLOR_YELLOW, COLOR_BLACK)) | A_BOLD);
        mvprintw(startY + 2, startX + 2 * (boxWidth + spacing) + 2, "> %s", displayName.c_str());
        attroff(COLOR_PAIR(getColorPair(COLOR_YELLOW, COLOR_BLACK)) | A_BOLD);
    } else {
        drawText(startX + 2 * (boxWidth + spacing) + 2, startY + 2, displayName, playerBoxColor);
    }
    
    // Draw instructions box with large spacing
    int instructionY = startY + boxHeight + 2;
    drawBox(startX, instructionY, 3 * boxWidth + 2 * spacing, 9, Color::BLUE);
    
    // Draw instructions title
    attron(COLOR_PAIR(getColorPair(COLOR_CYAN, COLOR_BLACK)) | A_BOLD);
    drawTextCentered(instructionY, "CONTROLS", Color::CYAN);
    attroff(A_BOLD);
    
    // Draw instructions with clear separation
    std::vector<std::pair<std::string, std::string>> instructions = {
        {"SELECT", "TAB key"},
        {"NAVIGATE", "Arrow keys"},
        {"CONFIRM", "Enter key"},
        {"EXIT", "Escape key"}
    };
    
    for (size_t i = 0; i < instructions.size(); i++) {
        int y = instructionY + 2 + static_cast<int>(i) * 2; // Double spacing
        
        // Draw label in bold yellow
        attron(COLOR_PAIR(getColorPair(COLOR_YELLOW, COLOR_BLACK)) | A_BOLD);
        mvprintw(y, startX + 4, "%s:", instructions[i].first.c_str());
        attroff(A_BOLD);
        
        // Draw instruction in white
        attron(COLOR_PAIR(getColorPair(COLOR_WHITE, COLOR_BLACK)));
        mvprintw(y, startX + 15, "%s", instructions[i].second.c_str());
        attroff(COLOR_PAIR(getColorPair(COLOR_WHITE, COLOR_BLACK)));
    }
    
    refresh();
}

IGraphicsLibrary::MouseEvent NcursesGraphics::getMouse()
{
    MouseEvent event = {0, 0, 0, false, false};

    return event;
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