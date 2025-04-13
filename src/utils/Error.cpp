/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** Error
*/

#include "utils/Error.hpp"

namespace arcd {

/**
 * @brief Constructor for ArcadeError
 * @param message error message to store
 */
ArcadeError::ArcadeError(const std::string& message) : _message(message) {}

/**
 * @brief Get the error message
 * @return C-style string containing the error message
 */
const char* ArcadeError::what() const noexcept {
    return _message.c_str();
}

}

