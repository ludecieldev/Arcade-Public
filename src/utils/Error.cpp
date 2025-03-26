/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** Error
*/

#include "utils/Error.hpp"

namespace arcd {

ArcadeError::ArcadeError(const std::string& message) : _message(message) {}

const char* ArcadeError::what() const noexcept {
    return _message.c_str();
}

}

