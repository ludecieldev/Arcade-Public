/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** Base interface for all modules
*/

#ifndef IMODULE_HPP_
    #define IMODULE_HPP_

#include <string>

namespace arcd {

    /**
     * @brief Base interface for all modules in the Arcade project.
     *
     * This interface defines the common methods that all modules (games and graphics)
     * must implement. It serves as a base interface for more specialized interfaces.
     */
    class IModule {
        public:
            virtual ~IModule() = default;

            /**
             * @brief Initializes the module.
             */
            virtual void init() = 0;

            /**
             * @brief Gets the name of the module.
             *
             * @return The name of the module as a string.
             */
            virtual const std::string &getName() const = 0;
    };

}

#endif /* !IMODULE_HPP_ */
