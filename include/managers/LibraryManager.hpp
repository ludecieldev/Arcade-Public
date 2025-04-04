/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** LibraryManager
*/

#pragma once

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

    // Function types for dynamic loading
    using create_graphics_t = IGraphicsLibrary* (*)();
    using destroy_graphics_t = void (*)(IGraphicsLibrary*);
    using create_game_t = IGameLibrary* (*)();
    using destroy_game_t = void (*)(IGameLibrary*);

    class LibraryManager {
        private:
            std::string _libDirectory;
            std::unique_ptr<ILibraryLoader> _graphicsLoader;
            std::unique_ptr<ILibraryLoader> _gameLoader;
            std::string _lastError;

            std::map<std::string, std::string> _graphicsLibs;
            std::map<std::string, std::string> _gameLibs;

            IGraphicsLibrary* _currentGraphicsLib;
            std::string _currentGraphicsLibPath;

            IGameLibrary* _currentGameLib;
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
            IGraphicsLibrary* getCurrentGraphicsLibrary() const;
            IGameLibrary* getCurrentGameLibrary() const;
            std::string getCurrentGraphicsLibraryName() const;
            std::string getCurrentGameLibraryName() const;

            // Error handling
            std::string getLastError() const;

        private:
            void discoverLibraries();
            bool isGraphicsLibrary(const std::string& path) const;
            bool isGameLibrary(const std::string& path) const;
    };

}
