/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** LibraryManager
*/

#ifndef LIBRARY_MANAGER_HPP
#define LIBRARY_MANAGER_HPP

#include "../interfaces/ILibraryLoader.hpp"
#include "../interfaces/IGraphicsLibrary.hpp"
#include "../interfaces/IGameLibrary.hpp"
#include "../utils/DLLoader.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <dlfcn.h>

namespace arcd {

    /**
     * @brief Function pointer types for dynamic loading with smart pointers
     */
    using create_graphics_t = std::unique_ptr<IGraphicsLibrary> (*)();
    using destroy_graphics_t = void (*)(IGraphicsLibrary*);
    using create_game_t = std::unique_ptr<IGameLibrary> (*)();
    using destroy_game_t = void (*)(IGameLibrary*);

    /**
     * @brief Manager class for dynamic library operations
     */
    class LibraryManager {
        private:
            /**
             * @brief Directory where libraries are stored
             */
            std::string _libDirectory;
            
            /**
             * @brief Loader for graphics libraries
             */
            std::unique_ptr<ILibraryLoader> _graphicsLoader;
            
            /**
             * @brief Loader for game libraries
             */
            std::unique_ptr<ILibraryLoader> _gameLoader;
            
            /**
             * @brief Last error message
             */
            std::string _lastError;

            /**
             * @brief Map of graphics library names to paths
             */
            std::map<std::string, std::string> _graphicsLibs;
            
            /**
             * @brief Map of game library names to paths
             */
            std::map<std::string, std::string> _gameLibs;

            /**
             * @brief Currently loaded graphics library
             */
            std::unique_ptr<IGraphicsLibrary> _currentGraphicsLib;
            
            /**
             * @brief Path to current graphics library
             */
            std::string _currentGraphicsLibPath;

            /**
             * @brief Currently loaded game library
             */
            std::unique_ptr<IGameLibrary> _currentGameLib;
            
            /**
             * @brief Path to current game library
             */
            std::string _currentGameLibPath;

            /**
             * @brief Index of current graphics library in the list
             */
            size_t _currentGraphicsIndex;
            
            /**
             * @brief Index of current game library in the list
             */
            size_t _currentGameIndex;

        public:
            /**
             * @brief Constructor for LibraryManager
             * @param libDirectory directory to scan for libraries
             */
            LibraryManager(const std::string& libDirectory);
            
            /**
             * @brief Destructor for LibraryManager
             */
            ~LibraryManager();

            /**
             * @brief Scan for available libraries in the directory
             */
            void scanLibraries();
            
            /**
             * @brief Get list of available graphics libraries
             * @return vector of graphics library names
             */
            std::vector<std::string> getGraphicsLibraries() const;
            
            /**
             * @brief Get list of available game libraries
             * @return vector of game library names
             */
            std::vector<std::string> getGameLibraries() const;

            /**
             * @brief Load a graphics library
             * @param path path to the library file
             * @return true if loading succeeded, false otherwise
             */
            bool loadGraphicsLibrary(const std::string& path);
            
            /**
             * @brief Load a game library
             * @param path path to the library file
             * @return true if loading succeeded, false otherwise
             */
            bool loadGameLibrary(const std::string& path);
            
            /**
             * @brief Unload the current graphics library
             */
            void unloadCurrentGraphicsLibrary();
            
            /**
             * @brief Unload the current game library
             */
            void unloadCurrentGameLibrary();

            /**
             * @brief Load the next graphics library in the list
             * @return true if loading succeeded, false otherwise
             */
            bool loadNextGraphicsLibrary();
            
            /**
             * @brief Load the next game library in the list
             * @return true if loading succeeded, false otherwise
             */
            bool loadNextGameLibrary();

            /**
             * @brief Get the current graphics library
             * @return reference to the current graphics library
             */
            IGraphicsLibrary& getCurrentGraphicsLibrary();
            
            /**
             * @brief Get the current game library
             * @return reference to the current game library
             */
            IGameLibrary& getCurrentGameLibrary();
            
            /**
             * @brief Get the name of the current graphics library
             * @return name of the current graphics library
             */
            std::string getCurrentGraphicsLibraryName() const { return _currentGraphicsLibPath; }
            
            /**
             * @brief Get the name of the current game library
             * @return name of the current game library
             */
            std::string getCurrentGameLibraryName() const { return _currentGameLibPath; }
            
            /**
             * @brief Check if a graphics library is loaded
             * @return true if a graphics library is loaded, false otherwise
             */
            bool hasGraphicsLibrary() const;
            
            /**
             * @brief Check if a game library is loaded
             * @return true if a game library is loaded, false otherwise
             */
            bool hasGameLibrary() const;

            /**
             * @brief Get the last error message
             * @return last error message
             */
            std::string getLastError() const;

        private:
            /**
             * @brief Discover available libraries
             */
            void discoverLibraries();
            
            /**
             * @brief Check if a file is a valid graphics library
             * @param path path to the file
             * @return true if it's a graphics library, false otherwise
             */
            bool isGraphicsLibrary(const std::string& path) const;
            
            /**
             * @brief Check if a file is a valid game library
             * @param path path to the file
             * @return true if it's a game library, false otherwise
             */
            bool isGameLibrary(const std::string& path) const;
    };

}

#endif // LIBRARY_MANAGER_HPP
