/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** Error
*/

#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>
#include <exception>

namespace arcd {

    /**
     * @brief Custom exception class for arcade-specific errors
     */
    class ArcadeError : public std::exception {
        private:
            /**
             * @brief Error message
             */
            std::string _message;

        public:
            /**
             * @brief Constructor for ArcadeError
             * @param message error message
             */
            explicit ArcadeError(const std::string& message);
            
            /**
             * @brief Get the error message
             * @return C-string containing the error message
             */
            const char* what() const noexcept override;
    };

}

#endif // ERROR_HPP