/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** Interface for game modules
*/

#ifndef IGAMEMODULE_HPP_
    #define IGAMEMODULE_HPP_

#include "IGraphicsModule.hpp"
#include "Types.hpp"

namespace arcd {

    /**
     * @brief Interface for game modules in the Arcade project.
     *
     * This interface defines the essential methods that all game modules must implement.
     * It provides a standardized way for the core to interact with different games.
     */
    class IGameModule {
        public:
            virtual ~IGameModule() = default;

            /**
             * @brief Initializes the game module.
             */
            virtual void init() = 0;

            /**
             * @brief Updates the game state.
             *
             * @param dt The time elapsed since the last update.
             */
            virtual void update(float dt) = 0;

            /**
             * @brief Renders the current game state.
             *
             * @param graphics The graphics module used for rendering.
             */
            virtual void render(IGraphicsModule &graphics) = 0;

            /**
             * @brief Checks if the game is over.
             *
             * @return true if the game is over, false otherwise.
             */
            virtual bool isGameOver() const = 0;

            /**
             * @brief Gets the current score of the game.
             *
             * @return The current score.
             */
            virtual int getScore() const = 0;

            /**
             * @brief Gets the name of the game.
             *
             * @return The name of the game as a string.
             */
            virtual const std::string &getName() const = 0;
    };

}

#endif /* !IGAMEMODULE_HPP_ */
