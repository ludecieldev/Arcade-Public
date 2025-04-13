#pragma once

#include <memory>
#include <string>
#include "IGraphic.hpp"

namespace arcd {

class IGame {
public:
    using Ptr = std::unique_ptr<IGame, void(*)(IGame*)>;

    virtual ~IGame() = default;

    // Game lifecycle
    virtual bool initialize() = 0;
    virtual void cleanup() = 0;
    virtual void reset() = 0;

    // Game logic
    virtual void update(const Input& input) = 0;
    virtual GameState getState() const = 0;

    // Game information
    virtual std::string getName() const = 0;
    virtual int getScore() const = 0;
    virtual bool isGameOver() const = 0;
    virtual bool isGameWon() const = 0;

    // Game settings
    virtual void setPlayerName(const std::string& name) = 0;
    virtual void setDifficulty(int difficulty) = 0;
};

} // namespace arcd

// Factory functions
extern "C" {
    arcd::IGame* create_game();
    void destroy_game(arcd::IGame*);
} 