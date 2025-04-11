/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** ICore - Interface for the Core component
*/

#ifndef I_CORE_HPP
#define I_CORE_HPP

#include "IGameLibrary.hpp"
#include "IGraphicsLibrary.hpp"
#include "IEvent.hpp"
#include "IGameState.hpp"
#include <string>
#include <memory>
#include <vector>

namespace arcd {

// Define core states
enum class CoreState {
    MENU,
    GAME,
    GAME_OVER,
    PAUSE,
    EXIT
};

// Interface for the Core component
class ICore {
public:
    virtual ~ICore() = default;

    // Core lifecycle
    virtual bool initialize() = 0;
    virtual void run() = 0;
    virtual void cleanup() = 0;

    // Library management
    virtual bool loadGameLibrary(const std::string& path) = 0;
    virtual bool loadGraphicsLibrary(const std::string& path) = 0;
    virtual bool switchGameLibrary(const std::string& name) = 0;
    virtual bool switchGraphicsLibrary(const std::string& name) = 0;
    
    // Get available libraries
    virtual std::vector<std::string> getAvailableGameLibraries() const = 0;
    virtual std::vector<std::string> getAvailableGraphicsLibraries() const = 0;

    // Core state
    virtual CoreState getState() const = 0;
    virtual void setState(CoreState state) = 0;
    
    // Event handling - the central hub for event processing
    virtual void processEvent(const IEvent& event) = 0;
    
    // Game state handling - the central hub for game state
    virtual const IGameState* getCurrentGameState() const = 0;
    
    // UI state
    virtual std::vector<UIElement> getUIElements() const = 0;
    
    // Player management
    virtual void setPlayerName(const std::string& name) = 0;
    virtual std::string getPlayerName() const = 0;
    
    // Score management
    virtual void saveScore(const std::string& gameName, int score) = 0;
    virtual std::vector<std::pair<std::string, int>> getHighScores(const std::string& gameName) const = 0;
};

} // namespace arcd

#endif // I_CORE_HPP 