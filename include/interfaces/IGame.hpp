/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** IGame
*/

#pragma once

#include <memory>
#include <string>
#include "IGraphic.hpp"

namespace arcd {

/**
 * @brief Interface for game libraries
 */
class IGame {
public:
    /**
     * @brief Smart pointer type for IGame with custom deleter
     */
    using Ptr = std::unique_ptr<IGame, void(*)(IGame*)>;

    /**
     * @brief Virtual destructor
     */
    virtual ~IGame() = default;

    /**
     * @brief Initialize the game
     * @return true if initialization succeeded, false otherwise
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Clean up resources used by the game
     */
    virtual void cleanup() = 0;
    
    /**
     * @brief Reset the game to its initial state
     */
    virtual void reset() = 0;

    /**
     * @brief Update game state based on input
     * @param input current input state
     */
    virtual void update(const Input& input) = 0;
    
    /**
     * @brief Get current game state
     * @return current game state
     */
    virtual GameState getState() const = 0;

    /**
     * @brief Get the name of the game
     * @return name of the game
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Get the current score
     * @return current score
     */
    virtual int getScore() const = 0;
    
    /**
     * @brief Check if the game is over
     * @return true if game is over, false otherwise
     */
    virtual bool isGameOver() const = 0;
    
    /**
     * @brief Check if the game is won
     * @return true if game is won, false otherwise
     */
    virtual bool isGameWon() const = 0;

    /**
     * @brief Set the player name
     * @param name player name
     */
    virtual void setPlayerName(const std::string& name) = 0;
    
    /**
     * @brief Set the game difficulty
     * @param difficulty difficulty level
     */
    virtual void setDifficulty(int difficulty) = 0;
};

} // namespace arcd

/**
 * @brief Factory functions for creating and destroying game instances
 */
extern "C" {
    /**
     * @brief Create a new game instance
     * @return pointer to the created game
     */
    arcd::IGame* create_game();
    
    /**
     * @brief Destroy a game instance
     * @param game pointer to the game to destroy
     */
    void destroy_game(arcd::IGame*);
} 