#include "graphics/Allegro5Graphics.hpp"
#include <iostream>
#include <filesystem>

namespace arcd {

Allegro5Graphics::Allegro5Graphics()
    : _initialized(false)
    , _width(1024)
    , _height(768)
    , _playerName("Player")
    , _lastKey(-1)
    , _frameCounter(0)
    , _display(nullptr)
    , _eventQueue(nullptr)
    , _font(nullptr)
    , _timer(nullptr)
    , _shouldClose(false)
{
}

Allegro5Graphics::~Allegro5Graphics() {
    cleanup();
}

bool Allegro5Graphics::initialize() {
    if (_initialized) {
        return true;
    }

    // Initialiser Allegro5
    if (!al_init()) {
        std::cerr << "Failed to initialize Allegro5" << std::endl;
        return false;
    }

    // Installer les addons nécessaires
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_install_keyboard();
    
    // Activer le support Unicode pour la saisie
    al_set_keyboard_leds(0);

    // Créer la fenêtre
    _display = al_create_display(_width, _height);
    if (!_display) {
        std::cerr << "Failed to create display" << std::endl;
        return false;
    }

    // Créer la file d'événements
    _eventQueue = al_create_event_queue();
    if (!_eventQueue) {
        std::cerr << "Failed to create event queue" << std::endl;
        return false;
    }

    // Créer le timer
    _timer = al_create_timer(1.0 / 30.0);
    if (!_timer) {
        std::cerr << "Failed to create timer" << std::endl;
        return false;
    }

    // Enregistrer les sources d'événements
    al_register_event_source(_eventQueue, al_get_display_event_source(_display));
    al_register_event_source(_eventQueue, al_get_keyboard_event_source());
    al_register_event_source(_eventQueue, al_get_timer_event_source(_timer));

    // Charger la police
    std::string fontPath = "assets/fonts/Arial.ttf";
    if (!std::filesystem::exists(fontPath)) {
        std::cerr << "Font file not found: " << fontPath << std::endl;
        return false;
    }

    _font = al_load_ttf_font(fontPath.c_str(), 24, 0);
    if (!_font) {
        std::cerr << "Failed to load font" << std::endl;
        return false;
    }

    // Initialiser les couleurs
    initColors();

    // Démarrer le timer
    al_start_timer(_timer);

    _initialized = true;
    _shouldClose = false;
    return true;
}

void Allegro5Graphics::cleanup() {
    if (_timer) {
        al_destroy_timer(_timer);
        _timer = nullptr;
    }
    if (_eventQueue) {
        al_destroy_event_queue(_eventQueue);
        _eventQueue = nullptr;
    }
    if (_font) {
        al_destroy_font(_font);
        _font = nullptr;
    }
    if (_display) {
        al_destroy_display(_display);
        _display = nullptr;
    }
    _initialized = false;
}

void Allegro5Graphics::clear() {
    if (!_initialized) return;
    al_clear_to_color(al_map_rgb(0, 0, 0));
}

void Allegro5Graphics::refresh() {
    if (!_initialized) return;
    al_flip_display();
}

int Allegro5Graphics::getKey() {
    if (!_initialized) return -1;

    ALLEGRO_EVENT event;
    while (al_get_next_event(_eventQueue, &event)) {
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            _shouldClose = true;
            return KEY_ESC_CODE;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (event.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    return KEY_UP_CODE;
                case ALLEGRO_KEY_DOWN:
                    return KEY_DOWN_CODE;
                case ALLEGRO_KEY_LEFT:
                    return KEY_LEFT_CODE;
                case ALLEGRO_KEY_RIGHT:
                    return KEY_RIGHT_CODE;
                case ALLEGRO_KEY_ENTER:
                    return KEY_ENTER_CODE;
                case ALLEGRO_KEY_ESCAPE:
                    _shouldClose = true;
                    return KEY_ESC_CODE;
                case ALLEGRO_KEY_SPACE:
                    return KEY_SPACE_CODE;
                case ALLEGRO_KEY_TAB:
                    return KEY_TAB_CODE;
                case ALLEGRO_KEY_BACKSPACE:
                    return KEY_BACKSPACE_CODE;
                case ALLEGRO_KEY_9:
                    return KEY_NEXT_LIB_CODE;
                case ALLEGRO_KEY_7:
                    return KEY_NEXT_GAME_CODE;
                default:
                    if (event.keyboard.keycode >= ALLEGRO_KEY_A && event.keyboard.keycode <= ALLEGRO_KEY_Z) {
                        return 'a' + (event.keyboard.keycode - ALLEGRO_KEY_A);
                    }
                    break;
            }
        }
    }
    return -1;
}

void Allegro5Graphics::getPlayerName(std::string& playerName) {
    if (!_initialized) {
        playerName = _playerName;
        return;
    }
    
    // Faire une copie du nom actuel pour édition
    std::string newName = _playerName;
    bool done = false;
    bool redraw = true;
    
    // Installer le clavier
    if (!al_is_keyboard_installed()) {
        al_install_keyboard();
    }
    
    // Pour gérer le curseur clignotant
    bool cursorVisible = true;
    
    // Créer un timer pour le clignotement du curseur
    ALLEGRO_TIMER* cursorTimer = al_create_timer(0.5); // 500ms interval
    if (!cursorTimer) {
        playerName = _playerName;
        return;
    }
    
    // Ajouter le timer à la file d'événements
    ALLEGRO_EVENT_QUEUE* nameEventQueue = al_create_event_queue();
    if (!nameEventQueue) {
        al_destroy_timer(cursorTimer);
        playerName = _playerName;
        return;
    }
    
    al_register_event_source(nameEventQueue, al_get_keyboard_event_source());
    al_register_event_source(nameEventQueue, al_get_timer_event_source(cursorTimer));
    al_register_event_source(nameEventQueue, al_get_display_event_source(_display));
    
    al_start_timer(cursorTimer);
    
    // Créer une police plus grande pour l'édition du nom
    ALLEGRO_FONT* nameFont = al_load_ttf_font("assets/fonts/Arial.ttf", 36, 0);
    if (!nameFont) {
        nameFont = _font; // Fallback to regular font
    }
    
    // Dessiner la boîte de dialogue et attendre la saisie
    while (!done) {
        ALLEGRO_EVENT event;
        
        // Redessiner si nécessaire
        if (redraw) {
            redraw = false;
            
            // Effacer l'écran
            clear();
            
            // Dessiner un fond semi-transparent
            al_draw_filled_rectangle(0, 0, _width, _height, al_map_rgba(0, 0, 0, 200));
            
            // Dessiner la boîte de dialogue
            al_draw_filled_rectangle(
                _width/4, _height/3,
                _width*3/4, _height*2/3,
                al_map_rgb(30, 30, 30)
            );
            
            al_draw_rectangle(
                _width/4, _height/3,
                _width*3/4, _height*2/3,
                al_map_rgb(200, 200, 200), 2.0f
            );
            
            // Dessiner le titre
            al_draw_text(
                nameFont, al_map_rgb(255, 255, 255),
                _width/2, _height/3 + 30,
                ALLEGRO_ALIGN_CENTER, "Enter Player Name"
            );
            
            // Dessiner le nom actuel avec un curseur clignotant
            std::string displayText = newName;
            if (cursorVisible) {
                displayText += "_";
            }
            
            al_draw_text(
                nameFont, al_map_rgb(255, 255, 0),
                _width/2, _height/2 - 18, // Centrer verticalement
                ALLEGRO_ALIGN_CENTER, displayText.c_str()
            );
            
            // Dessiner les instructions
            al_draw_text(
                _font, al_map_rgb(200, 200, 200),
                _width/2, _height*2/3 - 40,
                ALLEGRO_ALIGN_CENTER, "Press ENTER to confirm, ESC to cancel"
            );
            
            // Rafraîchir l'écran
            refresh();
        }
        
        // Vérifier si un événement est disponible
        bool hasEvent = al_get_next_event(nameEventQueue, &event);
        
        // Si pas d'événement, on continue
        if (!hasEvent) {
            continue;
        }
        
        if (event.type == ALLEGRO_EVENT_TIMER) {
            // Le timer du curseur a déclenché
            if (event.timer.source == cursorTimer) {
                cursorVisible = !cursorVisible;
                redraw = true;
            }
        }
        else if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
            // Une touche avec un caractère a été pressée
            int keycode = event.keyboard.keycode;
            int unichar = event.keyboard.unichar;
            
            if (keycode == ALLEGRO_KEY_ESCAPE) {
                // Annuler et garder l'ancien nom
                done = true;
            }
            else if (keycode == ALLEGRO_KEY_ENTER || keycode == ALLEGRO_KEY_PAD_ENTER) {
                // Valider le nom si non vide
                if (!newName.empty()) {
                    _playerName = newName;
                    playerName = _playerName;
                    done = true;
                }
            }
            else if (keycode == ALLEGRO_KEY_BACKSPACE) {
                // Effacer le dernier caractère
                if (!newName.empty()) {
                    newName.pop_back();
                    redraw = true;
                }
            }
            else if (unichar >= 32 && unichar < 127 && newName.length() < 20) {
                // Ajouter le caractère au nom
                newName += static_cast<char>(unichar);
                redraw = true;
            }
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            // La fenêtre a été fermée
            done = true;
        }
    }
    
    // Nettoyer les ressources
    al_stop_timer(cursorTimer);
    al_destroy_timer(cursorTimer);
    al_destroy_event_queue(nameEventQueue);
    
    if (nameFont && nameFont != _font) {
        al_destroy_font(nameFont);
    }
    
    // Mettre à jour le nom du joueur dans la référence passée
    playerName = _playerName;
    
    // Vider la file d'événements principale pour éviter que la touche ENTER
    // ne soit immédiatement traitée dans le menu principal
    ALLEGRO_EVENT flushEvent;
    while (al_get_next_event(_eventQueue, &flushEvent)) {
        // Ignorer tous les événements
    }
    
    // Petit délai pour éviter les actions immédiates
    al_rest(0.2);
}

std::string Allegro5Graphics::getName() const {
    return "Allegro5";
}

int Allegro5Graphics::getWidth() const {
    return _width;
}

int Allegro5Graphics::getHeight() const {
    return _height;
}

void Allegro5Graphics::drawText(int x, int y, const std::string& text, Color color) {
    if (!_initialized || !_font) return;
    
    // Calculer la hauteur de la ligne pour l'espacement
    int lineHeight = al_get_font_line_height(_font);
    
    // Diviser le texte en lignes
    std::string line;
    size_t pos = 0;
    int currentY = y;
    
    while (pos < text.length()) {
        size_t newline = text.find('\n', pos);
        if (newline == std::string::npos) {
            line = text.substr(pos);
            al_draw_text(_font, _colors[color], x, currentY, ALLEGRO_ALIGN_LEFT, line.c_str());
            break;
        } else {
            line = text.substr(pos, newline - pos);
            al_draw_text(_font, _colors[color], x, currentY, ALLEGRO_ALIGN_LEFT, line.c_str());
            currentY += lineHeight + 10; // Ajout d'espacement supplémentaire ici
            pos = newline + 1;
        }
    }
}

void Allegro5Graphics::drawBox(int x, int y, int width, int height, Color color) {
    if (!_initialized) return;
    al_draw_rectangle(x, y, x + width, y + height, _colors[color], 2.0f);
}

void Allegro5Graphics::drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color) {
    if (!_initialized || !_font) return;
    
    int lineHeight = al_get_font_line_height(_font) + 10; // Ajouter 10 pixels d'espacement
    for (size_t i = 0; i < items.size(); ++i) {
        Color itemColor = (static_cast<int>(i) == selectedIndex) ? Color::YELLOW : color;
        al_draw_text(_font, _colors[itemColor], x, y + (i * lineHeight), ALLEGRO_ALIGN_LEFT, items[i].c_str());
    }
}

void Allegro5Graphics::initColors() {
    _colors[Color::BLACK] = al_map_rgb(0, 0, 0);
    _colors[Color::RED] = al_map_rgb(255, 0, 0);
    _colors[Color::GREEN] = al_map_rgb(0, 255, 0);
    _colors[Color::YELLOW] = al_map_rgb(255, 255, 0);
    _colors[Color::BLUE] = al_map_rgb(0, 0, 255);
    _colors[Color::MAGENTA] = al_map_rgb(255, 0, 255);
    _colors[Color::CYAN] = al_map_rgb(0, 255, 255);
    _colors[Color::WHITE] = al_map_rgb(255, 255, 255);
    _colors[Color::DEFAULT] = al_map_rgb(255, 255, 255);
}

void Allegro5Graphics::updateWindowSize() {
    if (_initialized) {
        _width = al_get_display_width(_display);
        _height = al_get_display_height(_display);
    }
}

void Allegro5Graphics::drawTestScreen() {
    if (!_initialized) return;
    clear();
    
    // Titre centré
    int titleWidth = al_get_text_width(_font, "ARCADE");
    drawText(_width / 2 - titleWidth / 2, 50, "ARCADE", Color::WHITE);
    
    // Boîtes de menu avec espacement
    drawMenuBox("Games", {"Snake", "Nibbler", "Pacman"}, 100, 250, 400, 250, true); // Augmentation de la hauteur
    drawMenuBox("Graphics", {"Allegro5", "SDL2", "Ncurses"}, 350, 150, 200, 250, false); // Augmentation de la hauteur
    drawMenuBox("Player Name", {_playerName}, 600, 150, 200, 250, false); // Augmentation de la hauteur
    
    // Instructions avec espacement
    int instructionY = 420; // Ajustement de la position de départ
    drawText(100, instructionY, "Use TAB to switch between boxes", Color::WHITE);
    instructionY += al_get_font_line_height(_font) + 10;
    drawText(100, instructionY, "Use ARROWS to navigate", Color::WHITE);
    instructionY += al_get_font_line_height(_font) + 10;
    drawText(100, instructionY, "Press ENTER to select", Color::WHITE);
    
    refresh();
}

void Allegro5Graphics::drawMenuBox(const std::string& title, const std::vector<std::string>& items, int x, int y, int width, int height, bool isSelected) {
    Color boxColor = isSelected ? Color::YELLOW : Color::WHITE;
    drawBox(x, y, width, height, boxColor);
    
    // Titre avec espacement
    drawText(x + 10, y + 20, title, boxColor); // Ajustement de la position du titre
    
    // Items avec espacement
    int itemY = y + 60; // Ajustement de la position du premier item
    int lineSpacing = al_get_font_line_height(_font) + 10; // Augmentation de l'espacement
    for (const auto& item : items) {
        drawText(x + 10, itemY, item, boxColor);
        itemY += lineSpacing;
    }
}

void Allegro5Graphics::drawMenu(
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
    
    // Titre centré avec grande taille
    ALLEGRO_FONT* titleFont = al_load_ttf_font("assets/fonts/Arial.ttf", 36, 0);
    if (titleFont) {
        // Utiliser le retour du centre de l'alignement pour le texte
        al_draw_text(titleFont, _colors[Color::WHITE], _width / 2, 50, ALLEGRO_ALIGN_CENTER, title.c_str());
        al_destroy_font(titleFont);
    } else {
        // Fallback si la police de titre ne peut pas être chargée
        int titleWidth = al_get_text_width(_font, title.c_str());
        drawText(_width / 2 - titleWidth / 2, 50, title, Color::WHITE);
    }
    
    // Calcul des positions et dimensions pour une meilleure disposition
    const int boxWidth = 250;  // Augmentation de la largeur
    const int boxHeight = 300; // Augmentation de la hauteur
    const int margin = 50;
    const int boxesY = 120;
    
    // Position des boîtes centrées horizontalement
    const int totalWidth = 3 * boxWidth + 2 * margin;
    const int startX = (_width - totalWidth) / 2;
    
    // Dessiner les boîtes de menu
    bool isGameSelected = (selectedMenu == 0);
    bool isGraphicsSelected = (selectedMenu == 1);
    bool isPlayerSelected = (selectedMenu == 2);

    // Préparer les noms simplifiés pour l'affichage
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

    // Boîte des jeux
    drawBox(startX, boxesY, boxWidth, boxHeight, isGameSelected ? Color::YELLOW : Color::WHITE);
    drawText(startX + 10, boxesY + 20, "Games", isGameSelected ? Color::YELLOW : Color::WHITE);
    int itemY = boxesY + 60;
    int lineSpacing = al_get_font_line_height(_font) + 15; // Espacement augmenté
    for (size_t i = 0; i < displayGameNames.size(); i++) {
        Color itemColor = (isGameSelected && static_cast<int>(i) == selectedGameIndex) ? Color::YELLOW : Color::WHITE;
        drawText(startX + 20, itemY, displayGameNames[i], itemColor);
        itemY += lineSpacing;
    }
    
    // Boîte des graphiques
    drawBox(startX + boxWidth + margin, boxesY, boxWidth, boxHeight, isGraphicsSelected ? Color::YELLOW : Color::WHITE);
    drawText(startX + boxWidth + margin + 10, boxesY + 20, "Graphics", isGraphicsSelected ? Color::YELLOW : Color::WHITE);
    itemY = boxesY + 60;
    for (size_t i = 0; i < displayGraphicNames.size(); i++) {
        Color itemColor = (isGraphicsSelected && static_cast<int>(i) == selectedGraphicIndex) ? Color::YELLOW : Color::WHITE;
        drawText(startX + boxWidth + margin + 20, itemY, displayGraphicNames[i], itemColor);
        itemY += lineSpacing;
    }
    
    // Boîte du joueur
    drawBox(startX + 2 * (boxWidth + margin), boxesY, boxWidth, boxHeight, isPlayerSelected ? Color::YELLOW : Color::WHITE);
    drawText(startX + 2 * (boxWidth + margin) + 10, boxesY + 20, "Player Name", isPlayerSelected ? Color::YELLOW : Color::WHITE);
    drawText(startX + 2 * (boxWidth + margin) + 20, boxesY + 60, playerName, isPlayerSelected ? Color::YELLOW : Color::WHITE);
    
    // Instructions - utilisation d'une approche avec des boîtes distinctes pour chaque instruction
    const int instructionsStartY = boxesY + boxHeight + 50;
    const int instructionHeight = 40;
    const int instructionSpacing = 15;
    
    // Dessiner les boîtes d'instructions avec texte
    std::vector<std::pair<std::string, std::string>> instructions = {
        {"SELECT", "Use TAB to switch between boxes"},
        {"NAVIGATE", "Use ARROWS to navigate options"},
        {"CONFIRM", "Press ENTER to select option"},
        {"EXIT", "Press ESC to exit"}
    };
    
    for (size_t i = 0; i < instructions.size(); i++) {
        int y = instructionsStartY + i * (instructionHeight + instructionSpacing);
        
        // Dessiner une boîte de fond pour toute l'instruction
        al_draw_filled_rectangle(
            startX, y, 
            startX + totalWidth, y + instructionHeight, 
            al_map_rgba(50, 50, 50, 150)
        );
        
        // Dessiner le label
        drawText(startX + 20, y + 8, instructions[i].first + ":", Color::YELLOW);
        
        // Dessiner le texte d'instruction
        drawText(startX + 150, y + 8, instructions[i].second, Color::WHITE);
    }

    refresh();
}

} // namespace arcd

extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary() {
        return std::make_unique<arcd::Allegro5Graphics>();
    }
}
