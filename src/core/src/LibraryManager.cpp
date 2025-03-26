/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** LibraryManager class implementation
*/

#include "LibraryManager.hpp"
#include <filesystem>
#include <iostream>

namespace arcd {

    LibraryManager::LibraryManager()
    {
        scanLibraries();
    }

    std::shared_ptr<ILibrary> LibraryManager::loadGraphicsLibrary(const std::string &path)
    {
        try {
            std::unique_ptr<DLLoader<ILibrary>> loader = std::make_unique<DLLoader<ILibrary>>(path);
            return loader->getInstance();
        } catch (const std::exception &e) {
            throw std::runtime_error("Failed to load graphics library: " + std::string(e.what()));
        }
    }

    std::shared_ptr<IGameModule> LibraryManager::loadGameLibrary(const std::string &path)
    {
        try {
            std::unique_ptr<DLLoader<IGameModule>> loader = std::make_unique<DLLoader<IGameModule>>(path);
            return loader->getInstance();
        } catch (const std::exception &e) {
            throw std::runtime_error("Failed to load game library: " + std::string(e.what()));
        }
    }

    const std::vector<std::string> &LibraryManager::getAvailableGraphicsLibraries() const
    {
        return graphicsLibraries;
    }

    const std::vector<std::string> &LibraryManager::getAvailableGameLibraries() const
    {
        return gameLibraries;
    }

    void LibraryManager::scanLibraries()
    {
        const std::string libDir = "./lib/";

        try {
            for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(libDir))
                if (entry.path().extension() == ".so") {
                    std::string path = entry.path().string();
                    if (isGraphicsLibrary(path))
                        graphicsLibraries.push_back(path);
                    else if (isGameLibrary(path))
                        gameLibraries.push_back(path);
                }
        } catch (const std::exception &e) {
            std::cerr << "Warning: Failed to scan libraries directory: " << e.what() << std::endl;
        }
    }

    bool LibraryManager::isGraphicsLibrary(const std::string &path)
    {
        std::string filename = std::filesystem::path(path).filename().string();

        return (filename.find("arcade_") == 0 &&
               (filename.find("ncurses.so") != std::string::npos ||
                filename.find("sdl2.so") != std::string::npos ||
                filename.find("sfml.so") != std::string::npos));
    }

    bool LibraryManager::isGameLibrary(const std::string &path)
    {
        std::string filename = std::filesystem::path(path).filename().string();

        return (filename.find("arcade_") == 0 &&
               (filename.find("snake.so") != std::string::npos ||
                filename.find("pacman.so") != std::string::npos));
    }

}
