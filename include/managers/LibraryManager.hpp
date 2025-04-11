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

    // Function types for dynamic loading with smart pointers
    using create_graphics_t = std::unique_ptr<IGraphicsLibrary> (*)();
    using destroy_graphics_t = void (*)(IGraphicsLibrary*);
    using create_game_t = std::unique_ptr<IGameLibrary> (*)();
    using destroy_game_t = void (*)(IGameLibrary*);

    class LibraryManager {
        private:
            std::string _libDirectory;
            std::unique_ptr<ILibraryLoader> _graphicsLoader;
            std::unique_ptr<ILibraryLoader> _gameLoader;
            std::string _lastError;

            std::map<std::string, std::string> _graphicsLibs;
            std::map<std::string, std::string> _gameLibs;

            std::unique_ptr<IGraphicsLibrary> _currentGraphicsLib;
            std::string _currentGraphicsLibPath;

            std::unique_ptr<IGameLibrary> _currentGameLib;
            std::string _currentGameLibPath;

            size_t _currentGraphicsIndex;
            size_t _currentGameIndex;

        public:
            LibraryManager(const std::string& libDirectory);
            ~LibraryManager();

            // Library discovery
            void scanLibraries();
            std::vector<std::string> getGraphicsLibraries() const;
            std::vector<std::string> getGameLibraries() const;

            // Library loading
            bool loadGraphicsLibrary(const std::string& path);
            bool loadGameLibrary(const std::string& path);
            void unloadCurrentGraphicsLibrary();
            void unloadCurrentGameLibrary();

            // Library switching
            bool loadNextGraphicsLibrary();
            bool loadNextGameLibrary();

            // Library access
            IGraphicsLibrary& getCurrentGraphicsLibrary();
            IGameLibrary& getCurrentGameLibrary();
            std::string getCurrentGraphicsLibraryName() const {
                if (!_currentGraphicsLib) {
                    return "None";
                }
                
                std::string fullPath = _currentGraphicsLibPath;
                size_t arcade_ = fullPath.find("arcade_");
                size_t dotSo = fullPath.find(".so");
                
                if (arcade_ != std::string::npos && dotSo != std::string::npos) {
                    // Extraire le nom entre "arcade_" et ".so"
                    return fullPath.substr(arcade_ + 7, dotSo - arcade_ - 7);
                }
                
                return _currentGraphicsLib->getName();
            }
            std::string getCurrentGameLibraryName() const {
                if (!_currentGameLib) {
                    return "None";
                }
                
                std::string fullPath = _currentGameLibPath;
                size_t arcade_ = fullPath.find("arcade_");
                size_t dotSo = fullPath.find(".so");
                
                if (arcade_ != std::string::npos && dotSo != std::string::npos) {
                    // Extraire le nom entre "arcade_" et ".so"
                    return fullPath.substr(arcade_ + 7, dotSo - arcade_ - 7);
                }
                
                return _currentGameLib->getName();
            }
            
            // Check if libraries are loaded
            bool hasGraphicsLibrary() const;
            bool hasGameLibrary() const;

            // Error handling
            std::string getLastError() const;

        private:
            void discoverLibraries();
            bool isGraphicsLibrary(const std::string& path) const;
            bool isGameLibrary(const std::string& path) const;
    };

}

#endif // LIBRARY_MANAGER_HPP
