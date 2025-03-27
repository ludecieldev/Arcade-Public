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

    class ArcadeError : public std::exception {
        private:
            std::string _message;

        public:
            explicit ArcadeError(const std::string& message);
            const char* what() const noexcept override;
    };

}

#endif // ERROR_HPP