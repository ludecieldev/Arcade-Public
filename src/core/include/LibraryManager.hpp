/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** LibraryManager class header
*/

#ifndef LIBRARYMANAGER_HPP_
    #define LIBRARYMANAGER_HPP_

#include <string>
#include <vector>
#include <memory>
#include "DLLoader.hpp"
#include "ILibrary.hpp"
#include "IGameModule.hpp"

namespace arcd {

    /**
     * @brief Class for managing dynamic libraries loading.
     *
     * This class handles the loading, unloading, and management of
     * graphics and game libraries.
     */
    class LibraryManager {
        public:
            LibraryManager();
            ~LibraryManager() = default;

            /**
             * @brief Loads a graphics library.
             *
             * @param path The path to the library file.
             * @return A pointer to the loaded library.
             * @throw std::runtime_error If the library cannot be loaded.
             */
            std::shared_ptr<ILibrary> loadGraphicsLibrary(const std::string &path);

            /**
             * @brief Loads a game library.
             *
             * @param path The path to the library file.
             * @return A pointer to the loaded game.
             * @throw std::runtime_error If the library cannot be loaded.
             */
            std::shared_ptr<IGameModule> loadGameLibrary(const std::string &path);

            /**
             * @brief Gets a list of available graphics libraries.
             *
             * @return A vector of paths to available graphics libraries.
             */
            const std::vector<std::string> &getAvailableGraphicsLibraries() const;

            /**
             * @brief Gets a list of available game libraries.
             *
             * @return A vector of paths to available game libraries.
             */
            const std::vector<std::string> &getAvailableGameLibraries() const;

        private:
            /**
             * @brief Scans the lib directory for available libraries.
             */
            void scanLibraries();

            /**
             * @brief Checks if a library is a graphics library.
             *
             * @param path The path to the library.
             * @return true if it's a graphics library, false otherwise.
             */
            bool isGraphicsLibrary(const std::string &path);

            /**
             * @brief Checks if a library is a game library.
             *
             * @param path The path to the library.
             * @return true if it's a game library, false otherwise.
             */
            bool isGameLibrary(const std::string &path);

            std::vector<std::string> graphicsLibraries;
            std::vector<std::string> gameLibraries;
    };

}

#endif /* !LIBRARYMANAGER_HPP_ */
