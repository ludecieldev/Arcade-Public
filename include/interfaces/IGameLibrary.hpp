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

/**
 * @brief Interface for game libraries
 */
class IGameLibrary {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~IGameLibrary() = default;

    /**
     * @brief Initialize the game
     */
    virtual void initialize() = 0;
    
    /**
     * @brief Update game state
     */
    virtual void update() = 0;
    
    /**
     * @brief Restart the game to initial state
     */
    virtual void restart() = 0;
    
    /**
     * @brief Clean up resources used by the game
     */
    virtual void cleanup() = 0;

    /**
     * @brief Handle input events
     * @param key key code of the pressed key
     */
    virtual void handleInput(int key) = 0;

    /**
     * @brief Render the game using the provided graphics library
     * @param graphicsLib graphics library to use for rendering
     */
    virtual void render(IGraphicsLibrary& graphicsLib) = 0;

    /**
     * @brief Check if the game is over
     * @return true if game is over, false otherwise
     */
    virtual bool isGameOver() const = 0;
    
    /**
     * @brief Get the current score
     * @return current score
     */
    virtual int getScore() const = 0;
    
    /**
     * @brief Get the name of the game
     * @return name of the game
     */
    virtual std::string getName() const = 0;
};

} // namespace arcd

/**
 * @brief Factory functions for creating and destroying game library instances
 */
extern "C" {
    /**
     * @brief Create a new game library instance
     * @return unique pointer to the created game library
     */
    std::unique_ptr<arcd::IGameLibrary> createGameLibrary();
    
    /**
     * @brief Destroy a game library instance (kept for compatibility)
     * @param gameLib pointer to the game library to destroy
     */
    void destroyGameLibrary(arcd::IGameLibrary* gameLib);
}

#endif // I_GAME_LIBRARY_HPP