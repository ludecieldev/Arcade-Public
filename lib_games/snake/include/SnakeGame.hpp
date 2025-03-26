/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** Snake game implementation
*/

#ifndef SNAKEGAME_HPP_
    #define SNAKEGAME_HPP_

#include <string>
#include <vector>
#include "IGameModule.hpp"
#include "IGraphicsModule.hpp"

namespace arcd {

    class SnakeGame : public IGameModule {
        public:
            SnakeGame();
            ~SnakeGame() override = default;

            void init() override;
            void update(float deltaTime) override;
            void render(IGraphicsModule &graphics) override;
            bool isGameOver() const override;
            int getScore() const override;
            const std::string &getName() const override;

        private:
            std::string name;
            int score;
            bool gameOver;
    };

}

#endif /* !SNAKEGAME_HPP_ */
