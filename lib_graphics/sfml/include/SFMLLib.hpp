/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** SFML graphics library implementation
*/

#ifndef SFMLLIB_HPP_
    #define SFMLLIB_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <map>
#include <memory>
#include "IGraphicsModule.hpp"

namespace arcd {

    class SFMLLib : public IGraphicsModule {
        public:
            SFMLLib();
            ~SFMLLib() override;

            void init() override;
            void stop() override;
            void clear() override;
            void display() override;
            void drawText(const std::string& text, int x, int y) override;
            void drawShape(ShapeType type, int x, int y, int width, int height) override;
            KeysEvent getEvent() override;
            const std::string& getName() const override;

        private:
            sf::RenderWindow* window;
            sf::Font font;
            std::string name;
            std::map<int, sf::Texture> textures;
            int nextTextureId;
    };

}

#endif /* !SFMLLIB_HPP_ */