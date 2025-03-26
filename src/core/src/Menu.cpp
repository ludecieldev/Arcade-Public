/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** Menu class implementation
*/

#include "Menu.hpp"
#include <filesystem>

namespace arcd {

    Menu::Menu()
        : selectedGame(0),
          selectedGraphics(0),
          playerName("Player"),
          isEditingName(false)
    {
    }

    void Menu::display(IGraphicsModule &graphics,
                      const std::vector<std::string> &graphicsLibs,
                      const std::vector<std::string> &gameLibs,
                      const std::string &currentGraphics,
                      const std::string &currentGame,
                      const std::string &playerName)
    {
        graphics.clear();

        drawBox(graphics, 10, 5, 30, 15, "Games");
        for (size_t i = 0; i < gameLibs.size(); ++i) {
            std::string filename = std::filesystem::path(gameLibs[i]).filename().string();
            std::string displayName = filename.substr(0, filename.find_last_of('.'));
            std::string prefix = (i == selectedGame) ? "> " : "  ";
            graphics.drawText(prefix + displayName, 12, 7 + i);
        }

        drawBox(graphics, 50, 5, 30, 15, "Graphics Libraries");
        for (size_t i = 0; i < graphicsLibs.size(); ++i) {
            std::string filename = std::filesystem::path(graphicsLibs[i]).filename().string();
            std::string displayName = filename.substr(0, filename.find_last_of('.'));
            std::string prefix = (i == selectedGraphics) ? "> " : "  ";
            graphics.drawText(prefix + displayName, 52, 7 + i);
        }

        drawBox(graphics, 30, 25, 30, 5, "Player Name");
        std::string nameDisplay = isEditingName ? playerName + "_" : playerName;
        graphics.drawText(nameDisplay, 32, 27);

        drawBox(graphics, 10, 35, 70, 5, "Current Selections");
        graphics.drawText("Graphics: " + currentGraphics, 12, 37);
        graphics.drawText("Game: " + currentGame, 12, 38);

        drawBox(graphics, 10, 45, 70, 7, "Controls");
        graphics.drawText("Arrow keys: Navigate", 12, 47);
        graphics.drawText("Enter: Select", 12, 48);
        graphics.drawText("Tab: Switch between games and graphics", 12, 49);
        graphics.drawText("Escape: Exit", 12, 50);

        graphics.display();
    }

    bool Menu::handleEvent(KeysEvent event)
    {
        if (isEditingName) {
            if (event == KeysEvent::KeyEnter)
                isEditingName = false;
            else if (event == KeysEvent::KeyBackspace && !playerName.empty())
                playerName.pop_back();
            else if (event >= KeysEvent::KeyA && event <= KeysEvent::KeyZ) {
                char c = 'a' + (static_cast<int>(event) - static_cast<int>(KeysEvent::KeyA));

                playerName += c;
            } else if (event == KeysEvent::KeySpace)
                playerName += ' ';
            return false;
        }

        switch (event) {
            case KeysEvent::KeyUp:
                if (selectedGame > 0)
                    --selectedGame;
                break;
            case KeysEvent::KeyDown:
                ++selectedGame;
                break;
            case KeysEvent::KeyLeft:
                if (selectedGraphics > 0)
                    --selectedGraphics;
                break;
            case KeysEvent::KeyRight:
                ++selectedGraphics;
                break;
            case KeysEvent::KeyTab:
                // Switch
                break;
            case KeysEvent::KeyEnter:
                // Start
                return true;
            case KeysEvent::KeyEscape:
                // Exit
                break;
            default:
                break;
        }
        return false;
    }

    size_t Menu::getSelectedGame() const
    {
        return selectedGame;
    }

    size_t Menu::getSelectedGraphics() const
    {
        return selectedGraphics;
    }

    const std::string &Menu::getPlayerName() const
    {
        return playerName;
    }

    void Menu::drawBox(IGraphicsModule &graphics, int x, int y, int width, int height, const std::string &title)
    {
        graphics.drawShape(ShapeType::Rectangle, x, y, width, height);
        graphics.drawText(title, x + 2, y);
    }

}
