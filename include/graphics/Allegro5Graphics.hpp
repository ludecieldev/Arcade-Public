#pragma once

#include "../interfaces/IGraphicsLibrary.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <string>
#include <vector>
#include <map>

namespace arcd {

class Allegro5Graphics : public IGraphicsLibrary {
public:
    Allegro5Graphics();
    ~Allegro5Graphics() override;

    bool initialize() override;
    void cleanup() override;
    void clear() override;
    void refresh() override;
    int getKey() override;
    MouseEvent getMouse() override;
    void getPlayerName(std::string& playerName) override;
    std::string getName() const override;
    int getWidth() const override;
    int getHeight() const override;

    // Méthodes de dessin
    void drawText(int x, int y, const std::string& text, Color color = Color::DEFAULT) override;
    void drawBox(int x, int y, int width, int height, Color color = Color::DEFAULT) override;
    void drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color = Color::DEFAULT) override;

    // Menu interface standardisée
    void drawMenu(
        const std::string& title,
        const std::vector<std::string>& gameOptions,
        const std::vector<std::string>& graphicOptions,
        const std::string& playerName,
        int selectedMenu,
        int selectedGameIndex,
        int selectedGraphicIndex) override;

    // Test screen
    void drawTestScreen();

private:
    // État de base
    bool _initialized;
    int _width;
    int _height;
    std::string _playerName;
    int _lastKey;
    int _frameCounter;

    // Structures Allegro5
    ALLEGRO_DISPLAY* _display;
    ALLEGRO_EVENT_QUEUE* _eventQueue;
    ALLEGRO_FONT* _font;
    ALLEGRO_TIMER* _timer;
    bool _shouldClose;

    // Couleurs
    std::map<Color, ALLEGRO_COLOR> _colors;

    // Méthodes utilitaires
    void initColors();
    void updateWindowSize();
    void drawMenuBox(const std::string& title, const std::vector<std::string>& items, 
                    int x, int y, int width, int height, bool isSelected);
};

} // namespace arcd 