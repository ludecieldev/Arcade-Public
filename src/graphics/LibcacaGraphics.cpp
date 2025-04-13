/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** LibcacaGraphics
*/

#include "graphics/LibcacaGraphics.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <memory>

namespace arcd {

/**
 * @brief Constructeur de LibcacaGraphics
 * Initialise les dimensions de la fenêtre et la carte des couleurs
 */
LibcacaGraphics::LibcacaGraphics() : windowWidth(120), windowHeight(40) {
    initColorMap();
}

/**
 * @brief Destructeur de LibcacaGraphics
 * Les smart pointers gèrent automatiquement la libération des ressources
 */
LibcacaGraphics::~LibcacaGraphics() {
}

/**
 * @brief Obtient le nom de la bibliothèque graphique
 * @return Le nom de la bibliothèque (libcaca)
 */
std::string LibcacaGraphics::getName() const {
    return "libcaca";
}

/**
 * @brief Initialise la bibliothèque graphique libcaca
 * Crée le canvas et l'affichage, définit le titre et affiche un écran de démarrage
 * @return true si l'initialisation a réussi, false sinon
 */
bool LibcacaGraphics::initialize() {
    try {
        // Vérifier si la bibliothèque est déjà initialisée
        if (canvas && display) {
            return true; // Éviter l'initialisation multiple
        }

        // Si seulement l'un des deux objets existe, nettoyer avant de réinitialiser
        if (canvas || display) {
            cleanup();
        }

        caca_canvas_t* rawCanvas = caca_create_canvas(windowWidth, windowHeight);
        if (!rawCanvas) {
            throw std::runtime_error("Failed to create libcaca canvas");
        }
        canvas = CanvasPtr(rawCanvas);

        caca_display_t* rawDisplay = caca_create_display(canvas.get());
        if (!rawDisplay) {
            throw std::runtime_error("Failed to create libcaca display");
        }
        display = DisplayPtr(rawDisplay);

        setDisplayTitle("Arcade - libcaca");

        clear();
        setColorAnsi(CACA_WHITE, CACA_BLACK);
        putStr(windowWidth/2 - 5, windowHeight/2, "ARCADE");
        refresh();
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Réduit le temps d'attente

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error initializing libcaca: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Libère les ressources utilisées par la bibliothèque graphique
 * Réinitialise les smart pointers pour libérer les ressources
 */
void LibcacaGraphics::cleanup() {
    display.reset();
    canvas.reset();
}

/**
 * @brief Efface le contenu du canvas
 */
void LibcacaGraphics::clear() {
    if (canvas) {
        caca_clear_canvas(canvas.get());
    }
}

/**
 * @brief Rafraîchit l'affichage pour refléter les changements
 */
void LibcacaGraphics::refresh() {
    if (display) {
        caca_refresh_display(display.get());
    }
}

/**
 * @brief Dessine du texte à une position spécifique
 * @param x Position horizontale
 * @param y Position verticale
 * @param text Texte à dessiner
 * @param color Couleur du texte
 */
void LibcacaGraphics::drawText(int x, int y, const std::string& text, Color color) {
    if (!canvas) return;
    setColorAnsi(getColor(color), CACA_BLACK);
    putStr(x, y, text);
}

/**
 * @brief Dessine une boîte rectangulaire
 * @param x Position horizontale du coin supérieur gauche
 * @param y Position verticale du coin supérieur gauche
 * @param width Largeur de la boîte
 * @param height Hauteur de la boîte
 * @param color Couleur de la boîte
 */
void LibcacaGraphics::drawBox(int x, int y, int width, int height, Color color) {
    if (!canvas) return;
    setColorAnsi(getColor(color), CACA_BLACK);
    
    putChar(x, y, '+');
    putChar(x + width - 1, y, '+');
    putChar(x, y + height - 1, '+');
    putChar(x + width - 1, y + height - 1, '+');
    
    for (int i = x + 1; i < x + width - 1; ++i) {
        putChar(i, y, '-');
        putChar(i, y + height - 1, '-');
    }
    
    for (int i = y + 1; i < y + height - 1; ++i) {
        putChar(x, i, '|');
        putChar(x + width - 1, i, '|');
    }
}

/**
 * @brief Dessine une liste d'éléments avec un élément sélectionné
 * @param x Position horizontale
 * @param y Position verticale
 * @param items Liste des éléments à afficher
 * @param selectedIndex Index de l'élément sélectionné
 * @param color Couleur des éléments non sélectionnés
 */
void LibcacaGraphics::drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color) {
    if (!canvas) return;
    
    for (size_t i = 0; i < items.size(); ++i) {
        if (static_cast<int>(i) == selectedIndex) {
            setColorAnsi(CACA_WHITE, CACA_BLUE);
        } else {
            setColorAnsi(getColor(color), CACA_BLACK);
        }
        putStr(x, y + static_cast<int>(i), items[i]);
    }
}

/**
 * @brief Dessine le menu principal avec jeux, bibliothèques graphiques et options du joueur
 * @param title Titre du menu
 * @param games Liste des jeux disponibles
 * @param graphics Liste des bibliothèques graphiques disponibles
 * @param playerName Nom du joueur
 * @param selectedMenu Section sélectionnée (0=jeux, 1=graphiques, 2=joueur)
 * @param selectedGame Index du jeu sélectionné
 * @param selectedGraphic Index de la bibliothèque graphique sélectionnée
 * @param playerOptionSelected Option du joueur sélectionnée
 */
void LibcacaGraphics::drawMenu(
    const std::string& title,
    const std::vector<std::string>& games,
    const std::vector<std::string>& graphics,
    const std::string& playerName,
    int selectedMenu,
    int selectedGame,
    int selectedGraphic,
    int playerOptionSelected
) {
    if (!canvas) return;
    
    clear();
    
    setColorAnsi(CACA_WHITE, CACA_BLACK);
    std::string decoratedTitle = "=== " + title + " ===";
    putStr(windowWidth/2 - static_cast<int>(decoratedTitle.length())/2, 2, decoratedTitle);
    std::string playerInfo = "Player: " + playerName + " | Score: " + std::to_string(playerOptionSelected);
    putStr(windowWidth/2 - static_cast<int>(playerInfo.length())/2, 4, playerInfo);
    
    const int boxWidth = 35;
    const int boxHeight = 15;
    const int spacing = 6;
    const int startX = (windowWidth - (3 * boxWidth + 2 * spacing)) / 2;
    const int startY = 7;
    
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
        
        drawBox(x, startY, boxWidth, boxHeight, isSelected ? Color::YELLOW : Color::WHITE);
        
        setColorAnsi(isSelected ? CACA_YELLOW : CACA_WHITE, CACA_BLACK);
        putStr(x + 2, startY, boxTitles[box]);
        
        if (box < 2) {
            const auto& items = *boxContents[box];
            for (size_t i = 0; i < items.size() && i < static_cast<size_t>(boxHeight - 2); ++i) {
                bool isItemSelected = (isSelected && static_cast<int>(i) == selectedIndices[box]);
                std::string displayName = items[i];
                
                size_t prefixPos = displayName.find("arcade_");
                if (prefixPos != std::string::npos) {
                    displayName = displayName.substr(prefixPos + 7);
                }
                size_t extPos = displayName.find(".so");
                if (extPos != std::string::npos) {
                    displayName = displayName.substr(0, extPos);
                }
                
                if (isItemSelected) {
                    setColorAnsi(CACA_YELLOW, CACA_BLUE);
                    putStr(x + 2, startY + 2 + static_cast<int>(i), "> " + displayName);
                } else {
                    setColorAnsi(CACA_WHITE, CACA_BLACK);
                    putStr(x + 2, startY + 2 + static_cast<int>(i), displayName);
                }
            }
        } else {
            const char* playerOptions[] = {"Change Nickname", "Leaderboard"};
            for (int i = 0; i < 2; ++i) {
                bool isItemSelected = (isSelected && i == playerOptionSelected);
                if (isItemSelected) {
                    setColorAnsi(CACA_YELLOW, CACA_BLUE);
                    putStr(x + 2, startY + 2 + i, "> " + std::string(playerOptions[i]));
                } else {
                    setColorAnsi(CACA_WHITE, CACA_BLACK);
                    putStr(x + 2, startY + 2 + i, playerOptions[i]);
                }
            }
        }
    }
    
    const int instructionY = startY + boxHeight + 2;
    drawBox(startX, instructionY, 3 * boxWidth + 2 * spacing, 9, Color::BLUE);
    
    setColorAnsi(CACA_CYAN, CACA_BLACK);
    putStr(windowWidth/2 - 4, instructionY, "CONTROLS");
    
    const char* instructions[][2] = {
        {"SELECT", "TAB key"},
        {"NAVIGATE", "Arrow keys"},
        {"CONFIRM", "Enter key"},
        {"EXIT", "Escape key"}
    };
    
    for (int i = 0; i < 4; ++i) {
        int y = instructionY + 2 + i * 2;
        setColorAnsi(CACA_YELLOW, CACA_BLACK);
        putStr(startX + 2, y, std::string(instructions[i][0]) + ":");
        setColorAnsi(CACA_WHITE, CACA_BLACK);
        putStr(startX + 12, y, instructions[i][1]);
    }
    
    refresh();
}

/**
 * @brief Récupère une touche pressée par l'utilisateur
 * @return Code de la touche selon les constantes définies dans IGraphicsLibrary
 */
int LibcacaGraphics::getKey() {
    if (!display) return 0;

    int key = 0;
    if (getKeyEvent(key)) {
        switch (key) {
            case CACA_KEY_UP: return KEY_UP_CODE;
            case CACA_KEY_DOWN: return KEY_DOWN_CODE;
            case CACA_KEY_LEFT: return KEY_LEFT_CODE;
            case CACA_KEY_RIGHT: return KEY_RIGHT_CODE;
            case CACA_KEY_RETURN: return KEY_ENTER_CODE;
            case CACA_KEY_ESCAPE: return KEY_ESC_CODE;
            case CACA_KEY_BACKSPACE: return KEY_BACKSPACE_CODE;
            case CACA_KEY_DELETE: return KEY_BACKSPACE_CODE;
            case '\t': return KEY_TAB_CODE;
            case ' ': return KEY_SPACE_CODE;
            case '9': return KEY_NEXT_LIB_CODE;
            case '7': return KEY_NEXT_GAME_CODE;
            case 'r': return KEY_RESTART_GAME;
            case 'q': return KEY_ESC_CODE;
            default: 
                if (key >= ' ' && key <= '~') {
                    return key;
                }
                return key;
        }
    }
    return 0;
}

/**
 * @brief Permet à l'utilisateur de saisir son nom
 * @param playerName Référence au nom du joueur à modifier
 */
void LibcacaGraphics::getPlayerName(std::string& playerName) {
    if (!display) return;
    
    std::string originalName = playerName;
    std::string tempName = playerName;
    bool cursorVisible = true;
    auto lastBlinkTime = std::chrono::steady_clock::now();
    
    clear();
    setColorAnsi(CACA_WHITE, CACA_BLACK);
    putStr(windowWidth/2 - 20, windowHeight/2 - 4, "=== ENTER YOUR NAME ===");
    putStr(windowWidth/2 - 20, windowHeight/2 - 1, "Name: ");
    putStr(windowWidth/2 - 20, windowHeight/2 + 3, "ENTER: Confirm | ESC: Cancel | BACKSPACE: Delete");
    setColorAnsi(CACA_YELLOW, CACA_BLACK);
    putStr(windowWidth/2 - 14, windowHeight/2 - 1, tempName);
    refresh();
    
    while (true) {
        auto currentTime = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastBlinkTime).count() > 500) {
            cursorVisible = !cursorVisible;
            lastBlinkTime = currentTime;
            
            clear();
            setColorAnsi(CACA_WHITE, CACA_BLACK);
            putStr(windowWidth/2 - 20, windowHeight/2 - 4, "=== ENTER YOUR NAME ===");
            putStr(windowWidth/2 - 20, windowHeight/2 - 1, "Name: ");
            putStr(windowWidth/2 - 20, windowHeight/2 + 3, "ENTER: Confirm | ESC: Cancel | BACKSPACE: Delete");
            setColorAnsi(CACA_YELLOW, CACA_BLACK);
            
            std::string displayName = tempName;
            if (cursorVisible) {
                displayName += "_";
            }
            putStr(windowWidth/2 - 14, windowHeight/2 - 1, displayName);
            refresh();
        }
        
        int key = getKey();
        if (key == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }
        
        if (key == KEY_ENTER_CODE) {
            if (!tempName.empty()) {
                playerName = tempName;
            }
            break;
        } else if (key == KEY_ESC_CODE) {
            playerName = originalName;
            break;
        } else if (key == KEY_BACKSPACE_CODE || key == 8 || key == 127) {
            if (!tempName.empty()) {
                tempName.pop_back();
            }
        } else if (isprint(key) && tempName.length() < 20) {
            tempName += static_cast<char>(key);
        }
        
        clear();
        setColorAnsi(CACA_WHITE, CACA_BLACK);
        putStr(windowWidth/2 - 20, windowHeight/2 - 4, "=== ENTER YOUR NAME ===");
        putStr(windowWidth/2 - 20, windowHeight/2 - 1, "Name: ");
        putStr(windowWidth/2 - 20, windowHeight/2 + 3, "ENTER: Confirm | ESC: Cancel | BACKSPACE: Delete");
        setColorAnsi(CACA_YELLOW, CACA_BLACK);
        
        std::string displayName = tempName;
        if (cursorVisible) {
            displayName += "_";
        }
        putStr(windowWidth/2 - 14, windowHeight/2 - 1, displayName);
        refresh();
    }
    
    clear();
    refresh();
}

/**
 * @brief Obtient la largeur de la fenêtre
 * @return Largeur en caractères
 */
int LibcacaGraphics::getWidth() const {
    return windowWidth;
}

/**
 * @brief Obtient la hauteur de la fenêtre
 * @return Hauteur en caractères
 */
int LibcacaGraphics::getHeight() const {
    return windowHeight;
}

/**
 * @brief Initialise la carte des couleurs pour la conversion entre les énumérations Color et les valeurs libcaca
 */
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

/**
 * @brief Convertit une valeur Color en valeur de couleur libcaca
 * @param color Couleur à convertir
 * @return Valeur de couleur libcaca correspondante
 */
uint32_t LibcacaGraphics::getColor(Color color) const {
    auto it = colorMap.find(color);
    if (it != colorMap.end()) {
        return it->second;
    }
    return CACA_WHITE;
}

/**
 * @brief Définit les couleurs de premier plan et d'arrière-plan
 * @param fg Couleur de premier plan
 * @param bg Couleur d'arrière-plan
 */
void LibcacaGraphics::setColorAnsi(uint32_t fg, uint32_t bg) const {
    if (canvas) {
        caca_set_color_ansi(canvas.get(), fg, bg);
    }
}

/**
 * @brief Affiche un caractère à une position spécifique
 * @param x Position horizontale
 * @param y Position verticale
 * @param c Caractère à afficher
 */
void LibcacaGraphics::putChar(int x, int y, char c) const {
    if (canvas) {
        caca_put_char(canvas.get(), x, y, c);
    }
}

/**
 * @brief Affiche une chaîne de caractères à une position spécifique
 * @param x Position horizontale
 * @param y Position verticale
 * @param str Chaîne à afficher
 */
void LibcacaGraphics::putStr(int x, int y, const std::string& str) const {
    if (canvas) {
        caca_put_str(canvas.get(), x, y, str.c_str());
    }
}

/**
 * @brief Récupère un événement clavier et remplit la variable key
 * @param key Référence à la variable qui recevra le code de la touche
 * @return true si un événement clavier a été capturé, false sinon
 */
bool LibcacaGraphics::getKeyEvent(int& key) {
    if (caca_get_event(display.get(), CACA_EVENT_KEY_PRESS, &event, 0)) {
        key = getEventKeyCode(&event);
        return true;
    }
    return false;
}

/**
 * @brief Définit le titre de la fenêtre
 * @param title Titre à définir
 */
void LibcacaGraphics::setDisplayTitle(const std::string& title) const {
    if (display) {
        caca_set_display_title(display.get(), title.c_str());
    }
}

/**
 * @brief Obtient le code de touche à partir d'un événement
 * @param event Événement contenant le code de touche
 * @return Code de la touche
 */
int LibcacaGraphics::getEventKeyCode(const caca_event_t* event) const {
    return caca_get_event_key_ch(event);
}

} // namespace arcd

/**
 * @brief Fonction C externe pour créer une instance de la bibliothèque graphique
 * @return Smart pointer vers l'instance créée
 */
extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary() {
        return std::make_unique<arcd::LibcacaGraphics>();
    }

    /**
     * @brief Fonction C externe pour détruire une instance de la bibliothèque graphique
     * @param library Pointeur vers l'instance à détruire
     */
    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* library) {
        delete library;
    }
} 