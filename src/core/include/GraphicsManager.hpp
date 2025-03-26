/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** GraphicsManager class header
*/

#ifndef GRAPHICSMANAGER_HPP_
    #define GRAPHICSMANAGER_HPP_

#include <memory>
#include <string>
#include "LibraryManager.hpp"
#include "IGraphicsModule.hpp"

namespace arcd {

    /**
     * @brief Class for managing graphics libraries.
     *
     * This class handles the loading and interaction with graphics libraries.
     */
    class GraphicsManager {
        public:
            GraphicsManager();
            ~GraphicsManager() = default;

            /**
             * @brief Loads a graphics library.
             *
             * @param path The path to the library file.
             * @throw std::runtime_error If the library cannot be loaded.
             */
            void loadLibrary(const std::string &path);

            /**
             * @brief Switches to the next available graphics library.
             *
             * @return true if the switch was successful, false otherwise.
             */
            bool switchToNextLibrary();

            /**
             * @brief Gets the current graphics module.
             *
             * @return A reference to the current graphics module.
             */
            IGraphicsModule &getCurrentModule();

            /**
             * @brief Gets the name of the current graphics library.
             *
             * @return The name of the current graphics library.
             */
            const std::string &getCurrentLibraryName() const;

        private:
            std::unique_ptr<LibraryManager> libraryManager;
            std::shared_ptr<IGraphicsModule> currentModule;
            std::string currentLibraryPath;
            size_t currentLibraryIndex;
    };

}

#endif /* !GRAPHICSMANAGER_HPP_ */
