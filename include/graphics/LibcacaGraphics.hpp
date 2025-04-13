#ifndef LIBCACA_GRAPHICS_HPP
#define LIBCACA_GRAPHICS_HPP

#include "../interfaces/IGraphicsLibrary.hpp"
#include <caca.h>
#include <string>
#include <map>
#include <memory>

namespace arcd {
    class LibcacaGraphics : public IGraphicsLibrary {
    public:
        LibcacaGraphics();
        ~LibcacaGraphics();

        std::string getName() const override;
        bool initialize() override;
        void cleanup() override;
        void clear() override;
        void refresh() override;
        void drawText(int x, int y, const std::string& text, Color color = Color::DEFAULT) override;
        void drawBox(int x, int y, int width, int height, Color color = Color::DEFAULT) override;
        void drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color = Color::DEFAULT) override;
        void drawMenu(
            const std::string& title,
            const std::vector<std::string>& games,
            const std::vector<std::string>& graphics,
            const std::string& playerName,
            int selectedGame,
            int selectedGraphic,
            int selectedMenu,
            int score
        ) override;
        int getKey() override;
        void getPlayerName(std::string& playerName) override;
        int getWidth() const override;
        int getHeight() const override;

    private:
        caca_canvas_t* canvas;
        caca_display_t* display;
        caca_event_t event;
        int windowWidth;
        int windowHeight;
        std::map<Color, uint32_t> colorMap;

        void initColorMap();
        uint32_t getColor(Color color) const;
    };
}

extern "C" {
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary();
    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* library);
}

#endif // LIBCACA_GRAPHICS_HPP 