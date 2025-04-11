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
    
    // Initialiser la bibliothèque ncurses
    initscr();
    
    // Vider les buffers d'entrée
    flushinp();
    
    // Configurer ncurses pour l'utilisation des couleurs
    start_color();
    use_default_colors();
    
    // Configurer pour que getch() ne bloque pas
    nodelay(stdscr, TRUE);
    
    // Désactiver l'écho des touches pour éviter les doublons
    noecho();
    
    // Activer la lecture des touches spéciales (flèches, etc.)
    keypad(stdscr, TRUE);
    
    // Masquer le curseur par défaut
    curs_set(0);
    
    // Désactiver le buffering d'entrée (cause souvent des doublons)
    cbreak();
    
    // Désactiver le flush automatique (stdout et stderr)
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    
    // Récupérer la taille du terminal
    updateTerminalSize();
    
    // Initialiser les paires de couleurs
    init_pair(1, COLOR_BLACK, -1);    // NOIR sur fond par défaut
    init_pair(2, COLOR_RED, -1);      // ROUGE sur fond par défaut
    init_pair(3, COLOR_GREEN, -1);    // VERT sur fond par défaut
    init_pair(4, COLOR_YELLOW, -1);   // JAUNE sur fond par défaut
    init_pair(5, COLOR_BLUE, -1);     // BLEU sur fond par défaut
    init_pair(6, COLOR_MAGENTA, -1);  // MAGENTA sur fond par défaut
    init_pair(7, COLOR_CYAN, -1);     // CYAN sur fond par défaut
    init_pair(8, COLOR_WHITE, -1);    // BLANC sur fond par défaut
    
    // Afficher l'écran de démarrage
    showSplashScreen();
    
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
    if (!_initialized)
        return;

    // Vider le buffer d'entrée pour éviter les doublons initiaux
    flushinp();
    
    // Désactiver complètement l'écho des caractères
    noecho();
    
    // Créer une fenêtre d'entrée avec un fond coloré pour la visibilité
    WINDOW* inputWin = newwin(5, 50, (_height - 5) / 2, (_width - 50) / 2);
    box(inputWin, 0, 0);
    wbkgd(inputWin, COLOR_PAIR(getColorPair(COLOR_WHITE, COLOR_BLACK)));
    mvwprintw(inputWin, 1, 2, "Enter your name:");
    mvwprintw(inputWin, 3, 2, "Press Enter when done, Esc to cancel");
    
    // Zone d'entrée
    WINDOW* textWin = derwin(inputWin, 1, 40, 2, 5);
    wbkgd(textWin, COLOR_PAIR(getColorPair(COLOR_BLACK, COLOR_WHITE)));
    keypad(textWin, TRUE);
    
    // Affichage initial
    wrefresh(inputWin);
    wrefresh(textWin);
    
    // Variables pour suivre l'entrée
    playerName.clear();
    bool done = false;
    int cursor_x = 0;
    
    // Activer le curseur
    curs_set(1);
    
    // Boucle d'entrée
    while (!done) {
        // Positionner le curseur
        wmove(textWin, 0, cursor_x);
        wrefresh(textWin);
        
        // Obtenir l'entrée
        int ch = wgetch(textWin);
        
        // Traiter l'entrée
        switch (ch) {
            case 10:  // Enter
            case KEY_ENTER:
                done = true;
                break;
                
            case 27:  // Escape
                playerName.clear();
                done = true;
                break;
                
            case KEY_BACKSPACE:
            case 127:  // Backspace
                if (!playerName.empty()) {
                    playerName.pop_back();
                    cursor_x--;
                    
                    // Effacer le caractère
                    mvwaddch(textWin, 0, cursor_x, ' ');
                    wmove(textWin, 0, cursor_x);
                    wrefresh(textWin);
                }
                break;
                
            default:
                // N'accepter que les caractères imprimables
                if (isprint(ch) && playerName.length() < 20) {
                    playerName += ch;
                    mvwaddch(textWin, 0, cursor_x, ch);
                    cursor_x++;
                    wrefresh(textWin);
                }
                break;
        }
    }
    
    // Masquer le curseur et nettoyer
    curs_set(0);
    delwin(textWin);
    delwin(inputWin);
    
    // Nettoyer complètement le buffer d'entrée
    flushinp();
    
    // Retour à l'écran principal
    clear();
    refresh();
}

int NcursesGraphics::getKey()
{
    if (!_initialized)
        return 0;
    
    int ch = getch();
    
    if (ch == ERR)
        return 0;
    
    // Vider le tampon d'entrée pour éviter les échos de touches
    flushinp();
    
    return mapKeyCode(ch);
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
        int ch = getch();
        return mapKeyCode(ch);
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

// Map ncurses key code to our standard key codes
int NcursesGraphics::mapKeyCode(int ncursesKey)
{
    switch (ncursesKey) {
        case KEY_UP:            return KeyCode::UP;
        case KEY_DOWN:          return KeyCode::DOWN;
        case KEY_LEFT:          return KeyCode::LEFT;
        case KEY_RIGHT:         return KeyCode::RIGHT;
        case 27:                return KeyCode::ESC;
        case KEY_BACKSPACE:     return KeyCode::BACKSPACE;
        case 10:                return KeyCode::ENTER; // Return key
        case KEY_ENTER:         return KeyCode::ENTER; // Keypad Enter
        case ' ':               return KeyCode::SPACE;
        case '7':               return KeyCode::NEXT_GAME;
        case '8':               return KeyCode::PREV_GAME;
        case '9':               return KeyCode::NEXT_LIB;
        case '0':               return KeyCode::PREV_LIB;
        case 'r':
        case 'R':               return KeyCode::RESTART;
        case 'p':
        case 'P':               return KeyCode::PAUSE;
        case 'q':
        case 'Q':               return KeyCode::QUIT;
        default:                return ncursesKey;
    }
}

// NEW METHODS

std::optional<std::unique_ptr<IEvent>> NcursesGraphics::pollEvent()
{
    if (!_initialized) {
        return std::nullopt;
    }

    // Configurer le timeout pour nodelay
    timeout(0);
    
    // Récupérer une touche, ne bloque pas grâce au timeout(0)
    int ch = getch();
    
    // Si pas de touche, retourner nullopt
    if (ch == ERR) {
        return std::nullopt;
    }
    
    // Vider le tampon d'entrée pour éviter les doublons
    flushinp();
    
    // Si c'est une touche spéciale, la mapper sur notre système de codes
    int mappedKey = mapKeyCode(ch);
    
    // Créer et retourner un événement clavier
    return std::make_optional(IEvent::createKeyEvent(mappedKey, true));
}

void NcursesGraphics::renderEntity(const Entity& entity)
{
    if (!_initialized) return;
    
    // Convert string to Color enum
    Color color = Color::WHITE;
    if (entity.colorName == "BLACK") color = Color::BLACK;
    else if (entity.colorName == "RED") color = Color::RED;
    else if (entity.colorName == "GREEN") color = Color::GREEN;
    else if (entity.colorName == "YELLOW") color = Color::YELLOW;
    else if (entity.colorName == "BLUE") color = Color::BLUE;
    else if (entity.colorName == "MAGENTA") color = Color::MAGENTA;
    else if (entity.colorName == "CYAN") color = Color::CYAN;
    
    // Draw the entity
    if (entity.symbol.length() == 1) {
        // Single character entity
        attron(COLOR_PAIR(static_cast<int>(color)));
        for (int y = 0; y < entity.height; y++) {
            for (int x = 0; x < entity.width; x++) {
                mvaddch(entity.y + y, entity.x + x, entity.symbol[0]);
            }
        }
        attroff(COLOR_PAIR(static_cast<int>(color)));
    } else {
        // String entity (just display at position)
        drawText(entity.x, entity.y, entity.symbol, color);
    }
}

void NcursesGraphics::renderGameState(const IGameState& gameState)
{
    if (!_initialized) return;
    
    clear();  // Effacer l'écran avant de dessiner
    
    // Récupérer toutes les entités du jeu
    const auto& entities = gameState.getEntities();
    
    // Afficher chaque entité
    for (const auto& entity : entities) {
        // Obtenir la couleur de l'entité
        Color color = Color::WHITE;  // Couleur par défaut
        
        if (entity.colorName == "RED") color = Color::RED;
        else if (entity.colorName == "GREEN") color = Color::GREEN;
        else if (entity.colorName == "BLUE") color = Color::BLUE;
        else if (entity.colorName == "YELLOW") color = Color::YELLOW;
        else if (entity.colorName == "MAGENTA") color = Color::MAGENTA;
        else if (entity.colorName == "CYAN") color = Color::CYAN;
        else if (entity.colorName == "WHITE") color = Color::WHITE;
        
        // Utiliser le symbole de l'entité ou un caractère par défaut
        std::string symbol = entity.symbol.empty() ? "█" : entity.symbol;
        
        // Dessiner l'entité à sa position
        attron(COLOR_PAIR(static_cast<int>(color)));
        mvaddstr(entity.y, entity.x, symbol.c_str());
        attroff(COLOR_PAIR(static_cast<int>(color)));
    }
    
    // Si le jeu est terminé, afficher un message
    if (gameState.isGameOver()) {
        std::string message = "GAME OVER - Press 'R' to restart";
        drawTextCentered(_height / 2, message, Color::RED);
    }
    
    // Afficher le score dans un coin
    std::string scoreText = "Score: " + std::to_string(gameState.getScore());
    drawText(2, 1, scoreText, Color::YELLOW);
    
    // Rafraîchir l'écran
    refresh();
}

void NcursesGraphics::renderUI(const std::vector<UIElement>& uiElements)
{
    if (!_initialized) return;
    
    for (const auto& element : uiElements) {
        switch (element.type) {
            case UIElementType::TEXT: {
                if (element.selected) {
                    drawBoldText(element.x, element.y, element.text, element.color);
                } else {
                    drawText(element.x, element.y, element.text, element.color);
                }
                break;
            }
            case UIElementType::BUTTON: {
                int textX = element.x + (element.width - element.text.length()) / 2;
                int textY = element.y + element.height / 2;
                drawBox(element.x, element.y, element.width, element.height, element.color);
                if (element.selected) {
                    drawBoldText(textX, textY, element.text, element.color);
                } else {
                    drawText(textX, textY, element.text, element.color);
                }
                break;
            }
            case UIElementType::LIST: {
                auto items = std::any_cast<std::vector<std::string>>(
                    element.properties.at("items"));
                int selectedIndex = std::any_cast<int>(
                    element.properties.at("selectedIndex"));
                drawList(element.x, element.y, items, selectedIndex, element.color);
                break;
            }
            case UIElementType::PROGRESS_BAR: {
                int value = std::any_cast<int>(element.properties.at("value"));
                int maxValue = std::any_cast<int>(element.properties.at("maxValue"));
                drawProgressBar(element.x, element.y, element.width, value, maxValue, element.color);
                break;
            }
            case UIElementType::PANEL: {
                drawBox(element.x, element.y, element.width, element.height, element.color);
                if (!element.text.empty()) {
                    drawBoxWithTitle(element.x, element.y, element.width, element.height, element.text, element.color);
                }
                break;
            }
            case UIElementType::INPUT_FIELD: {
                drawBox(element.x, element.y, element.width, element.height, element.color);
                drawText(element.x + 1, element.y + element.height / 2, element.text, element.color);
                break;
            }
            default:
                break;
        }
    }
    
    refresh();
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