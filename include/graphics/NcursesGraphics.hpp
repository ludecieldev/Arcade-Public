/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** NcursesGraphics
*/

#ifndef NCURSES_GRAPHICS_HPP
#define NCURSES_GRAPHICS_HPP

#include "../interfaces/IGraphicsLibrary.hpp"
#include "../Core.hpp"
#include <string>
#include <vector>
#include <map>
#include <termios.h> // For terminal settings
#include <optional>
#include <functional>
#include <ncurses.h>
#include <memory>

namespace arcd {

/**
 * @brief Destructeur personnalisé pour les fenêtres Ncurses
 */
struct WindowDeleter {
    void operator()(WINDOW* w) { 
        if (w) delwin(w); 
    }
};

/**
 * @brief Type personnalisé pour les smart pointers de fenêtres Ncurses
 */
using WindowPtr = std::unique_ptr<WINDOW, WindowDeleter>;

/**
 * @brief Helper for mapping between game colors and ANSI terminal colors
 */
struct ColorMapper {
    /**
     * @brief Get ANSI color code for the specified color
     * @param color color to convert
     * @return string with ANSI escape sequence
     */
    static std::string getAnsiCode(Color color);
    
    /**
     * @brief Get bright ANSI color code for the specified color
     * @param color color to convert
     * @return string with bright ANSI escape sequence
     */
    static std::string getBrightAnsiCode(Color color);
    
    /**
     * @brief Get ANSI code for bold text
     * @return string with bold ANSI escape sequence
     */
    static std::string getBold();
    
    /**
     * @brief Get ANSI code to reset formatting
     * @return string with reset ANSI escape sequence
     */
    static std::string getReset();
};

/**
 * @brief Graphics library implementation using Ncurses
 */
class NcursesGraphics : public IGraphicsLibrary {
    private:
        /**
         * @brief Flag indicating if library has been initialized
         */
        bool _initialized;
        
        /**
         * @brief Width of the terminal display
         */
        int _width;
        
        /**
         * @brief Height of the terminal display
         */
        int _height;
        
        /**
         * @brief Last key pressed
         */
        int _lastKey;
        
        /**
         * @brief Counter for animation frames
         */
        int _frameCounter;
        
        /**
         * @brief Original terminal settings to restore on exit
         */
        struct termios _oldTermios;
        
        /**
         * @brief Map of color pairs for ncurses
         */
        std::map<int, int> _colorPairs;
        
        /**
         * @brief Optional reference to the core system
         */
        std::optional<std::reference_wrapper<Core>> _core;

        // Encapsulation des fonctions C natives de ncurses
        /**
         * @brief Encapsule l'initialisation de ncurses
         */
        void ncursesInit();
        
        /**
         * @brief Encapsule la fermeture de ncurses
         */
        void ncursesEnd();
        
        /**
         * @brief Encapsule la configuration du mode raw
         */
        void ncursesRaw();
        
        /**
         * @brief Encapsule la désactivation de l'echo
         */
        void ncursesNoEcho();
        
        /**
         * @brief Encapsule l'activation de l'echo
         */
        void ncursesEcho();
        
        /**
         * @brief Encapsule la configuration du curseur
         * @param visibility niveau de visibilité du curseur (0=invisible, 1=normal, 2=très visible)
         */
        void ncursesSetCursor(int visibility);
        
        /**
         * @brief Encapsule la configuration du clavier
         * @param window fenêtre pour laquelle configurer le clavier
         * @param enable true pour activer, false pour désactiver
         */
        void ncursesKeypad(WINDOW* window, bool enable);
        
        /**
         * @brief Encapsule la configuration du mode nodelay
         * @param window fenêtre pour laquelle configurer le mode
         * @param enable true pour activer, false pour désactiver
         */
        void ncursesNodelay(WINDOW* window, bool enable);
        
        /**
         * @brief Encapsule l'initialisation des couleurs
         */
        void ncursesStartColor();
        
        /**
         * @brief Encapsule l'utilisation des couleurs par défaut
         */
        void ncursesUseDefaultColors();
        
        /**
         * @brief Encapsule l'initialisation d'une paire de couleurs
         * @param pairIndex index de la paire à initialiser
         * @param foreground couleur d'avant-plan
         * @param background couleur d'arrière-plan
         */
        void ncursesInitPair(int pairIndex, int foreground, int background);
        
        /**
         * @brief Encapsule la récupération des dimensions de l'écran
         * @param win fenêtre dont on veut les dimensions
         * @param height référence pour stocker la hauteur
         * @param width référence pour stocker la largeur
         */
        void ncursesGetMaxYX(WINDOW* win, int& height, int& width);
        
        /**
         * @brief Encapsule l'activation d'un attribut
         * @param attrs attributs à activer
         */
        void ncursesAttron(int attrs);
        
        /**
         * @brief Encapsule la désactivation d'un attribut
         * @param attrs attributs à désactiver
         */
        void ncursesAttroff(int attrs);
        
        /**
         * @brief Encapsule l'affichage d'une chaîne à une position donnée
         * @param y position verticale
         * @param x position horizontale
         * @param str chaîne à afficher
         */
        void ncursesAddstr(int y, int x, const std::string& str);
        
        /**
         * @brief Encapsule l'affichage d'une chaîne formatée à une position donnée
         * @param y position verticale
         * @param x position horizontale
         * @param format chaîne de format
         * @param args arguments variables
         */
        template<typename... Args>
        void ncursesAddstr(int y, int x, const char* format, Args... args)
        {
            mvprintw(y, x, format, args...);
        }
        
        /**
         * @brief Encapsule l'affichage formaté
         * @param y position verticale
         * @param x position horizontale
         * @param format chaîne de format
         * @param args arguments variables
         */
        template<typename... Args>
        void ncursesPrintw(int y, int x, const char* format, Args... args)
        {
            mvprintw(y, x, format, args...);
        }
        
        /**
         * @brief Encapsule le déplacement du curseur
         * @param y position verticale
         * @param x position horizontale
         */
        void ncursesMove(int y, int x);
        
        /**
         * @brief Encapsule la récupération d'une touche
         * @return code de la touche ou ERR si aucune touche
         */
        int ncursesGetch();
        
        /**
         * @brief Encapsule la configuration du timeout
         * @param delay délai en millisecondes
         */
        void ncursesTimeout(int delay);
        
        /**
         * @brief Encapsule le vidage du buffer d'entrée
         */
        void ncursesFlushInput();
        
        /**
         * @brief Encapsule l'effacement de l'écran
         */
        void ncursesErase();

        /**
         * @brief Update terminal size information
         */
        void updateTerminalSize();
        
        /**
         * @brief Show splash screen on startup
         */
        void showSplashScreen();
        
        /**
         * @brief Wait for key press with timeout
         * @param timeoutMs timeout in milliseconds
         * @return key code or ERR if timeout
         */
        int waitForKey(int timeoutMs);
        
        /**
         * @brief Wait for any key press
         */
        void waitForAnyKey();

        /**
         * @brief Initialize color pairs for ncurses
         */
        void initColors();

        /**
         * @brief Get a color pair index for the given foreground and background
         * @param fg foreground color
         * @param bg background color
         * @return color pair index
         */
        int getColorPair(int fg, int bg);

        /**
         * @brief Clear input buffer
         */
        void flushInputBuffer();

        /**
         * @brief Draw centered text at specified y-coordinate
         * @param y y-coordinate
         * @param text text to draw
         * @param color color to use
         */
        void drawTextCentered(int y, const std::string& text, Color color = Color::DEFAULT);
        
        /**
         * @brief Draw box with a title
         * @param x x-coordinate
         * @param y y-coordinate
         * @param width width of the box
         * @param height height of the box
         * @param title title to display
         * @param color color to use
         */
        void drawBoxWithTitle(int x, int y, int width, int height, const std::string& title, Color color = Color::DEFAULT);
        
        /**
         * @brief Draw filled box with character
         * @param x x-coordinate
         * @param y y-coordinate
         * @param width width of the box
         * @param height height of the box
         * @param fillChar character to fill with
         * @param color color to use
         */
        void drawFilledBox(int x, int y, int width, int height, char fillChar, Color color = Color::DEFAULT);
        
        /**
         * @brief Draw horizontal line
         * @param x x-coordinate
         * @param y y-coordinate
         * @param width width of the line
         * @param color color to use
         */
        void drawHorizontalLine(int x, int y, int width, Color color = Color::DEFAULT);
        
        /**
         * @brief Draw vertical line
         * @param x x-coordinate
         * @param y y-coordinate
         * @param height height of the line
         * @param color color to use
         */
        void drawVerticalLine(int x, int y, int height, Color color = Color::DEFAULT);
        
        /**
         * @brief Draw progress bar
         * @param x x-coordinate
         * @param y y-coordinate
         * @param width width of the bar
         * @param value current value
         * @param maxValue maximum value
         * @param color color to use
         */
        void drawProgressBar(int x, int y, int width, int value, int maxValue, Color color = Color::DEFAULT);
        
        /**
         * @brief Draw bold text
         * @param x x-coordinate
         * @param y y-coordinate
         * @param text text to draw
         * @param color color to use
         */
        void drawBoldText(int x, int y, const std::string& text, Color color);

    public:
        /**
         * @brief Constructor for NcursesGraphics
         */
        NcursesGraphics();
        
        /**
         * @brief Destructor for NcursesGraphics
         */
        ~NcursesGraphics() override;

        /**
         * @brief Initialize the graphics library
         * @return true if initialization succeeded, false otherwise
         */
        bool initialize() override;
        
        /**
         * @brief Clean up resources used by the graphics library
         */
        void cleanup() override;

        /**
         * @brief Clear the screen
         */
        void clear() override;
        
        /**
         * @brief Refresh the display
         */
        void refresh() override;

        /**
         * @brief Draw text at the specified position
         * @param x x-coordinate
         * @param y y-coordinate
         * @param text the text to draw
         * @param color color to use for drawing
         */
        void drawText(int x, int y, const std::string& text, Color color = Color::DEFAULT) override;
        
        /**
         * @brief Draw a box at the specified position
         * @param x x-coordinate
         * @param y y-coordinate
         * @param width width of the box
         * @param height height of the box
         * @param color color to use for drawing
         */
        void drawBox(int x, int y, int width, int height, Color color = Color::DEFAULT) override;
        
        /**
         * @brief Draw a list of items with a selected index
         * @param x x-coordinate
         * @param y y-coordinate
         * @param items vector of strings to display
         * @param selectedIndex index of the selected item
         * @param color color to use for drawing
         */
        void drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color = Color::DEFAULT) override;

        /**
         * @brief Get the last key pressed
         * @return integer code of the key
         */
        int getKey() override;

        /**
         * @brief Get the player's name via user input
         * @param playerName reference to string to store the player name
         */
        void getPlayerName(std::string& playerName) override;

        /**
         * @brief Get the name of the graphics library
         * @return name of the graphics library
         */
        std::string getName() const override;

        /**
         * @brief Get the width of the display
         * @return width in characters
         */
        int getWidth() const override { return _width; }
        
        /**
         * @brief Get the height of the display
         * @return height in characters
         */
        int getHeight() const override { return _height; }

        /**
         * @brief Set a reference to the core system
         * @param core reference to the Core object
         */
        void setCore(Core& core) { _core = std::reference_wrapper<Core>(core); }

        /**
         * @brief Draw the menu screen
         * @param title title of the menu
         * @param gameOptions list of game options
         * @param graphicOptions list of graphic library options
         * @param playerName name of the player
         * @param selectedMenu selected menu section (0 for games, 1 for graphics)
         * @param selectedGameIndex selected game index
         * @param selectedGraphicIndex selected graphics library index
         */
        void drawMenu(
            const std::string& title,
            const std::vector<std::string>& gameOptions,
            const std::vector<std::string>& graphicOptions,
            const std::string& playerName,
            int selectedMenu,
            int selectedGameIndex,
            int selectedGraphicIndex,
            int playerOptionSelected = 0) override;
};

}

/**
 * @brief Factory function to create a graphics library instance
 * @return unique pointer to the created graphics library
 */
extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary();
    
    /**
     * @brief Function to destroy a graphics library instance
     * @param graphicsLib pointer to the graphics library to destroy
     */
    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* graphicsLib);
}

#endif // NCURSES_GRAPHICS_HPP