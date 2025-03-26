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

namespace arcd {

class IGameLibrary {
public:
    virtual ~IGameLibrary() = default;
    
    // Game lifecycle
    virtual bool initialize() = 0;
    virtual void cleanup() = 0;
    virtual void restart() = 0;
    
    // Game loop
    virtual void update(float deltaTime) = 0;
    virtual void render(IGraphicsLibrary* graphicsLib) = 0;
    
    // Input handling
    virtual void handleInput(int key) = 0;
    
    // Game state
    virtual bool isGameOver() const = 0;
    virtual int getScore() const = 0;
    
    // Game information
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
};

// Function signature for the create/destroy functions that must be in the library
typedef IGameLibrary* (*create_game_t)();
typedef void (*destroy_game_t)(IGameLibrary*);

}

#endif // I_GAME_LIBRARY_HPP