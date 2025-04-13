/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** IGameLibrary
*/

#ifndef I_GAME_LIBRARY_HPP
#define I_GAME_LIBRARY_HPP

#include "IGraphicsLibrary.hpp"
#include <string>
#include <memory>

namespace arcd {

class IGameLibrary {
public:
    virtual ~IGameLibrary() = default;

    // Game lifecycle
    virtual void initialize() = 0;
    virtual void update() = 0;
    virtual void restart() = 0;
    virtual void cleanup() = 0;

    // Input handling
    virtual void handleInput(int key) = 0;

    // Rendering
    virtual void render(IGraphicsLibrary& graphicsLib) = 0;

    // Game state
    virtual bool isGameOver() const = 0;
    virtual int getScore() const = 0;
    virtual std::string getName() const = 0;
};

} // namespace arcd

// Dynamic library entry points with smart pointers
extern "C" {
    std::unique_ptr<arcd::IGameLibrary> createGameLibrary();
    void destroyGameLibrary(arcd::IGameLibrary* gameLib); // Keep for compatibility
}

#endif // I_GAME_LIBRARY_HPP