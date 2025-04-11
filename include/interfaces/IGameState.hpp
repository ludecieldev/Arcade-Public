/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** IGameState - Interface for game state representation
*/

#ifndef I_GAME_STATE_HPP
#define I_GAME_STATE_HPP

#include <vector>
#include <string>
#include <memory>
#include <any>
#include <map>

namespace arcd {

// Common entity types in games
enum class EntityType {
    PLAYER,
    ENEMY,
    OBSTACLE,
    COLLECTIBLE,
    PROJECTILE,
    BACKGROUND,
    UI_ELEMENT,
    CUSTOM
};

// Entity representation
struct Entity {
    EntityType type;
    int x;
    int y;
    int width;
    int height;
    std::string symbol;    // Character or string to represent the entity
    std::string colorName; // Name of the color (corresponds to Color enum)
    std::map<std::string, std::any> properties; // Custom properties
};

// Game state interface
class IGameState {
public:
    virtual ~IGameState() = default;

    // Board dimensions
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;

    // Entity access
    virtual const std::vector<Entity>& getEntities() const = 0;
    
    // Game information
    virtual int getScore() const = 0;
    virtual bool isGameOver() const = 0;
    virtual std::string getMessage() const = 0; // For game messages
    
    // Custom state data (for game-specific needs)
    virtual std::any getCustomState(const std::string& key) const = 0;
};

} // namespace arcd

#endif // I_GAME_STATE_HPP 