/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** LibraryManager
*/

#include "managers/LibraryManager.hpp"
#include <filesystem>
#include <iostream>

namespace arcd {

LibraryManager::LibraryManager(const std::string& libDirectory)
    : _libDirectory(libDirectory), _currentGraphicsIndex(0), _currentGameIndex(0)
{
    _graphicsLoader = std::make_unique<DLLoader>();
    _gameLoader = std::make_unique<DLLoader>();
}

LibraryManager::~LibraryManager()
{
    unloadCurrentGraphicsLibrary();
    unloadCurrentGameLibrary();
}

void LibraryManager::scanLibraries()
{
    _graphicsLibs.clear();
    _gameLibs.clear();
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(_libDirectory)) {
            if (entry.path().extension() == ".so") {
                std::string path = entry.path().string();
                std::string name = entry.path().filename().string();
                
                // Try to load the library to check its type
                if (_graphicsLoader->load(path)) {
                    // Check if it's a graphics library
                    void* createGraphicsSymbol = _graphicsLoader->getSymbol("createGraphicsLibrary");
                    if (createGraphicsSymbol) {
                        _graphicsLibs[name] = path;
                    }
                    _graphicsLoader->unload();
                }
                
                if (_gameLoader->load(path)) {
                    // Check if it's a game library
                    void* createGameSymbol = _gameLoader->getSymbol("createGameLibrary");
                    if (createGameSymbol) {
                        _gameLibs[name] = path;
                    }
                    _gameLoader->unload();
                }
            }
        }
    } catch (const std::exception& e) {
        _lastError = "Error scanning libraries: " + std::string(e.what());
        std::cerr << _lastError << std::endl;
    }
}

std::vector<std::string> LibraryManager::getGraphicsLibraries() const
{
    std::vector<std::string> libs;
    for (const auto& [name, path] : _graphicsLibs) {
        libs.push_back(name);
    }
    return libs;
}

std::vector<std::string> LibraryManager::getGameLibraries() const
{
    std::vector<std::string> libs;
    for (const auto& [name, path] : _gameLibs) {
        libs.push_back(name);
    }
    return libs;
}

bool LibraryManager::loadGraphicsLibrary(const std::string& name)
{
    // Unload current library if any
    unloadCurrentGraphicsLibrary();
    
    // Find the library path
    std::string path;
    if (_graphicsLibs.find(name) != _graphicsLibs.end()) {
        path = _graphicsLibs[name];
    } else {
        // If not found in the map, try to use the name as a path
        path = name;
    }
    
    // Load the library
    if (!_graphicsLoader->load(path)) {
        _lastError = "Failed to load graphics library: " + _graphicsLoader->getError();
        return false;
    }
    
    // Get the create function using the type-safe template method
    auto createFunc = _graphicsLoader->getSymbolAs<create_graphics_t>("createGraphicsLibrary");
    if (!createFunc) {
        _lastError = "Invalid graphics library: " + _graphicsLoader->getError();
        _graphicsLoader->unload();
        return false;
    }
    
    // Create the graphics library instance
    _currentGraphicsLib = createFunc();
    if (!_currentGraphicsLib) {
        _lastError = "Failed to create graphics library instance";
        _graphicsLoader->unload();
        return false;
    }
    
    // Initialize the graphics library
    if (!_currentGraphicsLib->initialize()) {
        _lastError = "Failed to initialize graphics library";
        unloadCurrentGraphicsLibrary();
        return false;
    }
    
    _currentGraphicsLibPath = path;
    return true;
}

IGraphicsLibrary& LibraryManager::getCurrentGraphicsLibrary()
{
    if (!_currentGraphicsLib || !_graphicsLoader->isLoaded()) {
        throw std::runtime_error("No graphics library loaded");
    }
    
    return *_currentGraphicsLib;
}

bool LibraryManager::hasGraphicsLibrary() const
{
    return _currentGraphicsLib != nullptr && _graphicsLoader->isLoaded();
}

bool LibraryManager::loadNextGraphicsLibrary()
{
    // Implementation of graphics library switching
    if (_graphicsLibs.empty()) {
        _lastError = "No graphics libraries available";
        return false;
    }

    _currentGraphicsIndex = (_currentGraphicsIndex + 1) % _graphicsLibs.size();
    
    // Get the library name at the current index
    auto it = _graphicsLibs.begin();
    std::advance(it, _currentGraphicsIndex);
    return loadGraphicsLibrary(it->first);
}

bool LibraryManager::loadGameLibrary(const std::string& name)
{
    // Unload current library if any
    unloadCurrentGameLibrary();
    
    // Find the library path
    std::string path;
    if (_gameLibs.find(name) != _gameLibs.end()) {
        path = _gameLibs[name];
    } else {
        // If not found in the map, try to use the name as a path
        path = name;
    }
    
    // Load the library
    if (!_gameLoader->load(path)) {
        _lastError = "Failed to load game library: " + _gameLoader->getError();
        return false;
    }
    
    // Get the create function using type-safe method
    auto createFunc = _gameLoader->getSymbolAs<create_game_t>("createGameLibrary");
    if (!createFunc) {
        _lastError = "Invalid game library: " + _gameLoader->getError();
        _gameLoader->unload();
        return false;
    }
    
    // Create the game library instance
    _currentGameLib = createFunc();
    if (!_currentGameLib) {
        _lastError = "Failed to create game library instance";
        _gameLoader->unload();
        return false;
    }
    
    _currentGameLibPath = path;
    return true;
}

IGameLibrary& LibraryManager::getCurrentGameLibrary()
{
    if (!_currentGameLib || !_gameLoader->isLoaded()) {
        throw std::runtime_error("No game library loaded");
    }
    
    return *_currentGameLib;
}

bool LibraryManager::hasGameLibrary() const
{
    return _currentGameLib != nullptr && _gameLoader->isLoaded();
}

bool LibraryManager::loadNextGameLibrary()
{
    // Implementation of game library switching
    if (_gameLibs.empty()) {
        _lastError = "No game libraries available";
        return false;
    }

    _currentGameIndex = (_currentGameIndex + 1) % _gameLibs.size();
    
    // Get the library name at the current index
    auto it = _gameLibs.begin();
    std::advance(it, _currentGameIndex);
    return loadGameLibrary(it->first);
}

void LibraryManager::unloadCurrentGraphicsLibrary()
{
    if (_currentGraphicsLib) {
        try {
            // Call cleanup before destroying
            _currentGraphicsLib->cleanup();
        } catch (const std::exception& e) {
            std::cerr << "Warning: Error during graphics library cleanup: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Warning: Unknown error during graphics library cleanup" << std::endl;
        }
        
        // Reset the unique_ptr which will handle destruction
        _currentGraphicsLib.reset();
        _currentGraphicsLibPath = "";
        
        // Unload the actual shared library
        if (_graphicsLoader && _graphicsLoader->isLoaded()) {
            _graphicsLoader->unload();
        }
    }
}

void LibraryManager::unloadCurrentGameLibrary()
{
    if (_currentGameLib) {
        try {
            // Call cleanup before destroying to ensure proper resource release
            _currentGameLib->cleanup();
        } catch (const std::exception& e) {
            std::cerr << "Warning: Error during game library cleanup: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Warning: Unknown error during game library cleanup" << std::endl;
        }
        
        // Reset the unique_ptr which will handle destruction
        _currentGameLib.reset();
        _currentGameLibPath = "";
        
        // Unload the actual shared library
        if (_gameLoader && _gameLoader->isLoaded()) {
            _gameLoader->unload();
        }
    }
}

std::string LibraryManager::getLastError() const
{
    return _lastError;
}

} // namespace arcd
