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
    : _libDirectory(libDirectory), _currentGraphicsLib(nullptr), _currentGameLib(nullptr),
      _currentGraphicsIndex(0), _currentGameIndex(0)
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
    
    // Get the create function
    void* createSymbol = _graphicsLoader->getSymbol("createGraphicsLibrary");
    if (!createSymbol) {
        _lastError = "Invalid graphics library: " + _graphicsLoader->getError();
        _graphicsLoader->unload();
        return false;
    }
    
    // Create the graphics library instance
    auto createFunc = reinterpret_cast<create_graphics_t>(createSymbol);
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

IGraphicsLibrary* LibraryManager::getCurrentGraphicsLibrary() const
{
    if (!_currentGraphicsLib) {
        return nullptr;
    }
    
    // Verify the library is still valid
    if (!_graphicsLoader->isLoaded()) {
        return nullptr;
    }
    
    return _currentGraphicsLib;
}

bool LibraryManager::loadNextGraphicsLibrary()
{
    // Implementation of graphics library switching
    if (_graphicsLibs.empty()) {
        _lastError = "No graphics libraries available";
        return false;
    }

    _currentGraphicsIndex = (_currentGraphicsIndex + 1) % _graphicsLibs.size();
    return loadGraphicsLibrary(_graphicsLibs.begin()->first);
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
    
    // Get the create function
    void* createSymbol = _gameLoader->getSymbol("createGameLibrary");
    if (!createSymbol) {
        _lastError = "Invalid game library: " + _gameLoader->getError();
        _gameLoader->unload();
        return false;
    }
    
    // Create the game library instance
    auto createFunc = reinterpret_cast<create_game_t>(createSymbol);
    _currentGameLib = createFunc();
    if (!_currentGameLib) {
        _lastError = "Failed to create game library instance";
        _gameLoader->unload();
        return false;
    }
    
    _currentGameLibPath = path;
    return true;
}

IGameLibrary* LibraryManager::getCurrentGameLibrary() const
{
    return _currentGameLib;
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
        // Call cleanup before destroying
        _currentGraphicsLib->cleanup();
        
        // Get the destroy function
        if (_graphicsLoader->isLoaded()) {
            void* destroySymbol = _graphicsLoader->getSymbol("destroyGraphicsLibrary");
            if (destroySymbol) {
                auto destroyFunc = reinterpret_cast<destroy_graphics_t>(destroySymbol);
                destroyFunc(_currentGraphicsLib);
            }
        }
        
        _currentGraphicsLib = nullptr;
        _currentGraphicsLibPath = "";
        _graphicsLoader->unload();
    }
}

void LibraryManager::unloadCurrentGameLibrary()
{
    if (_currentGameLib) {
        // Get the destroy function
        if (_gameLoader->isLoaded()) {
            void* destroySymbol = _gameLoader->getSymbol("destroyGameLibrary");
            if (destroySymbol) {
                auto destroyFunc = reinterpret_cast<destroy_game_t>(destroySymbol);
                destroyFunc(_currentGameLib);
            }
        }
        
        _currentGameLib = nullptr;
        _currentGameLibPath = "";
        _gameLoader->unload();
    }
}

std::string LibraryManager::getLastError() const
{
    return _lastError;
}

} // namespace arcd
