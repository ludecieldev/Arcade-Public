/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** IGameLibrary
*/

#ifndef I_GAME_LIBRARY_HPP
#define I_GAME_LIBRARY_HPP

#include "IGameState.hpp"
#include "IEvent.hpp"
#include <string>
#include <memory>

namespace arcd {

class IGameLibrary {
public:
    virtual ~IGameLibrary() = default;

    // Game lifecycle
    virtual void initialize() = 0;
    virtual void update(double deltaTime) = 0;
    virtual void restart() = 0;
    virtual void cleanup() = 0;

    // Input handling - returns true if the event was handled
    virtual bool processEvent(const IEvent& event) = 0;

    // Game state - the main method to get the current state of the game
    virtual std::unique_ptr<IGameState> getGameState() const = 0;

    // Game information
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
};

} // namespace arcd

// Dynamic library entry points with smart pointers
extern "C" {
    std::unique_ptr<arcd::IGameLibrary> createGameLibrary();
    void destroyGameLibrary(arcd::IGameLibrary* gameLib); // Keep for compatibility
}

#endif // I_GAME_LIBRARY_HPP