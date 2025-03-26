/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** GraphicsManager class implementation
*/

#include "GraphicsManager.hpp"
#include "DLLoader.hpp"
#include <iostream>

namespace arcd {

    GraphicsManager::GraphicsManager()
        : libraryManager(std::make_unique<LibraryManager>()),
          currentLibraryIndex(0)
    {
    }

    void GraphicsManager::loadLibrary(const std::string &path)
    {
        try {
            if (currentModule)
                currentModule->stop();

            DLLoader<IGraphicsModule> loader(path);
            currentModule = loader.getInstance();

            if (!currentModule) // segfault ici à cause du throw, mauvaise gestion de base de currentmodule donc dans mon dlloader?
                throw std::runtime_error("Library does not provide a valid graphics module");

            currentLibraryPath = path;
            const std::vector<std::string> &libraries = libraryManager->getAvailableGraphicsLibraries();

            for (size_t i = 0; i < libraries.size(); ++i) {
                if (libraries[i] == path) {
                    currentLibraryIndex = i;
                    break;
                }
            }
            currentModule->init();
        } catch (const std::exception &e) {
            throw std::runtime_error("Failed to load graphics library: " + std::string(e.what()));
        }
    }


    bool GraphicsManager::switchToNextLibrary()
    {
        const std::vector<std::string> &libraries = libraryManager->getAvailableGraphicsLibraries();

        if (libraries.empty())
            return false;

        size_t nextIndex = (currentLibraryIndex + 1) % libraries.size();
        try {
            loadLibrary(libraries[nextIndex]);
            return true;
        } catch (const std::exception &e) {
            std::cerr << "Error switching graphics library: " << e.what() << std::endl;
            return false;
        }
    }

    IGraphicsModule &GraphicsManager::getCurrentModule()
    {
        if (!currentModule)
            throw std::runtime_error("No graphics module loaded");
        return *currentModule;
    }

    const std::string &GraphicsManager::getCurrentLibraryName() const
    {
        if (!currentModule)
            throw std::runtime_error("No graphics module loaded");
        return currentModule->getName();
    }

}
