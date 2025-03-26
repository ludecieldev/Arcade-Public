/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** SFML graphics library implementation
*/

#include "SFMLLib.hpp"
#include <iostream>

namespace arcd {

    SFMLLib::SFMLLib()
        : window(nullptr),
          name("SFML"),
          nextTextureId(0)
    {
    }

    SFMLLib::~SFMLLib()
    {
        stop();
    }

    void SFMLLib::init()
    {
        window = new sf::RenderWindow(sf::VideoMode(sf::Vector2u(800, 600)), "Arcade - SFML");
        window->setFramerateLimit(60);

        if (!font.openFromFile("assets/fonts/arial.ttf")) {
            std::cerr << "Error: Could not load font" << std::endl;
            // in error case -> try to load system font as fallback
            if (!font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
                std::cerr << "Error: Could not load fallback font" << std::endl;
            }
        }
    }

    void SFMLLib::stop()
    {
        if (window) {
            window->close();
            delete window;
            window = nullptr;
        }
    }

    void SFMLLib::clear()
    {
        if (window)
            window->clear(sf::Color::Black);
    }

    void SFMLLib::display()
    {
        if (window)
            window->display();
    }

    void SFMLLib::drawText(const std::string& text, int x, int y)
    {
        if (!window)
            return;
        sf::Text sfText(font, "", 18);

        sfText.setFont(font);
        sfText.setString(text);
        sfText.setCharacterSize(18);
        sfText.setFillColor(sf::Color::White);
        sfText.setPosition(sf::Vector2f(static_cast<float>(x), static_cast<float>(y)));
        window->draw(sfText);
    }

    void SFMLLib::drawShape(ShapeType type, int x, int y, int width, int height)
    {
        if (!window)
            return;

        switch (type) {
            case ShapeType::Rectangle: {
                sf::RectangleShape rectangle(sf::Vector2f(width, height));
                rectangle.setPosition(sf::Vector2f(x, y));
                rectangle.setFillColor(sf::Color::Transparent);
                rectangle.setOutlineColor(sf::Color::White);
                rectangle.setOutlineThickness(1);
                window->draw(rectangle);
                break;
            }
            case ShapeType::Circle: {
                sf::CircleShape circle(width / 2);
                circle.setPosition(sf::Vector2f(x, y));
                circle.setFillColor(sf::Color::Transparent);
                circle.setOutlineColor(sf::Color::White);
                circle.setOutlineThickness(1);
                window->draw(circle);
                break;
            }
            case ShapeType::Line: {
                sf::Vertex line[2];
                line[0].position = sf::Vector2f(x, y);
                line[0].color = sf::Color::White;
                line[1].position = sf::Vector2f(x + width, y + height);
                line[1].color = sf::Color::White;
                window->draw(line, 2, sf::PrimitiveType::Lines);
                break;
            }
            default:
                break;
        }
    }

    KeysEvent SFMLLib::getEvent()
    {
        if (!window)
            return KeysEvent::None;

        std::optional<sf::Event> optEvent = window->pollEvent();
        if (!optEvent)
            return KeysEvent::None;

        sf::Event event = *optEvent;

        if (event.is<sf::Event::Closed>())
            return KeysEvent::KeyEscape;

        if (auto keyEvent = event.getIf<sf::Event::KeyPressed>()) {
            switch (keyEvent->code) {
                case sf::Keyboard::Key::Up:
                    return KeysEvent::KeyUp;
                case sf::Keyboard::Key::Down:
                    return KeysEvent::KeyDown;
                case sf::Keyboard::Key::Left:
                    return KeysEvent::KeyLeft;
                case sf::Keyboard::Key::Right:
                    return KeysEvent::KeyRight;
                case sf::Keyboard::Key::Escape:
                    return KeysEvent::KeyEscape;
                case sf::Keyboard::Key::Enter:
                    return KeysEvent::KeyEnter;
                case sf::Keyboard::Key::Tab:
                    return KeysEvent::KeyTab;
                case sf::Keyboard::Key::Backspace:
                    return KeysEvent::KeyBackspace;
                case sf::Keyboard::Key::Space:
                    return KeysEvent::KeySpace;
                default:
                    if (keyEvent->code >= sf::Keyboard::Key::A && keyEvent->code <= sf::Keyboard::Key::Z) {
                        return static_cast<KeysEvent>(static_cast<int>(KeysEvent::KeyA) +
                            (static_cast<int>(keyEvent->code) - static_cast<int>(sf::Keyboard::Key::A)));
                    }
                    break;
            }
        }

        return KeysEvent::None;
    }

    const std::string& SFMLLib::getName() const
    {
        return name;
    }

}

extern "C"
{
    arcd::IGraphicsModule* create()
    {
        return new arcd::SFMLLib();
    }
}
