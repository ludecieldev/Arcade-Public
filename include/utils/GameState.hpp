/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** GameState - Concrete implementation of IGameState
*/

#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "../interfaces/IGameState.hpp"
#include <vector>
#include <string>
#include <map>
#include <any>

namespace arcd {

class GameState : public IGameState {
private:
    int _width;
    int _height;
    std::vector<Entity> _entities;
    int _score;
    bool _gameOver;
    std::string _message;
    std::map<std::string, std::any> _customState;

public:
    GameState(int width, int height)
        : _width(width), _height(height), _score(0), _gameOver(false) {}
    
    ~GameState() override = default;

    // Board dimensions
    int getWidth() const override { return _width; }
    int getHeight() const override { return _height; }

    // Entity access and manipulation
    const std::vector<Entity>& getEntities() const override { return _entities; }
    void addEntity(const Entity& entity) { _entities.push_back(entity); }
    void clearEntities() { _entities.clear(); }
    
    // Game information
    int getScore() const override { return _score; }
    void setScore(int score) { _score = score; }
    
    bool isGameOver() const override { return _gameOver; }
    void setGameOver(bool gameOver) { _gameOver = gameOver; }
    
    std::string getMessage() const override { return _message; }
    void setMessage(const std::string& message) { _message = message; }
    
    // Custom state data
    std::any getCustomState(const std::string& key) const override {
        auto it = _customState.find(key);
        return (it != _customState.end()) ? it->second : std::any();
    }
    
    void setCustomState(const std::string& key, const std::any& value) {
        _customState[key] = value;
    }
    
    // Helper method to create an entity
    static Entity createEntity(EntityType type, int x, int y, const std::string& symbol, 
                              const std::string& colorName = "WHITE", int width = 1, int height = 1) {
        Entity entity;
        entity.type = type;
        entity.x = x;
        entity.y = y;
        entity.width = width;
        entity.height = height;
        entity.symbol = symbol;
        entity.colorName = colorName;
        return entity;
    }
};

} // namespace arcd

#endif // GAME_STATE_HPP 