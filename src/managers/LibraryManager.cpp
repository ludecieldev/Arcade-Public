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
    : _libDirectory(libDirectory), _currentGraphicsLib(nullptr), _currentGameLib(nullptr)
{
    _loader = std::make_unique<DLLoader>();
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
                if (_loader->load(path)) {
                    // Check if it's a graphics library
                    void* createGraphicsSymbol = _loader->getSymbol("createGraphicsLibrary");
                    if (createGraphicsSymbol) {
                        _graphicsLibs[name] = path;
                    } else {
                        // Check if it's a game library
                        void* createGameSymbol = _loader->getSymbol("createGameLibrary");
                        if (createGameSymbol) {
                            _gameLibs[name] = path;
                        }
                    }
                    _loader->unload();
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
    if (!_loader->load(path)) {
        _lastError = "Failed to load graphics library: " + _loader->getError();
        return false;
    }
    
    // Get the create function
    void* createSymbol = _loader->getSymbol("createGraphicsLibrary");
    if (!createSymbol) {
        _lastError = "Invalid graphics library: " + _loader->getError();
        _loader->unload();
        return false;
    }
    
    // Create the graphics library instance
    auto createFunc = reinterpret_cast<create_graphics_t>(createSymbol);
    _currentGraphicsLib = createFunc();
    if (!_currentGraphicsLib) {
        _lastError = "Failed to create graphics library instance";
        _loader->unload();
        return false;
    }
    
    _currentGraphicsLibPath = path;
    return true;
}

IGraphicsLibrary* LibraryManager::getCurrentGraphicsLibrary() const
{
    return _currentGraphicsLib;
}

bool LibraryManager::switchToNextGraphicsLibrary()
{
    if (_graphicsLibs.empty()) {
        _lastError = "No graphics libraries available";
        return false;
    }
    
    // Find the current library in the map
    auto it = _graphicsLibs.begin();
    if (!_currentGraphicsLibPath.empty()) {
        for (; it != _graphicsLibs.end(); ++it) {
            if (it->second == _currentGraphicsLibPath) {
                ++it;
                break;
            }
        }
        
        // If we reached the end, wrap around
        if (it == _graphicsLibs.end()) {
            it = _graphicsLibs.begin();
        }
    }
    
    // Load the next library
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
    if (!_loader->load(path)) {
        _lastError = "Failed to load game library: " + _loader->getError();
        return false;
    }
    
    // Get the create function
    void* createSymbol = _loader->getSymbol("createGameLibrary");
    if (!createSymbol) {
        _lastError = "Invalid game library: " + _loader->getError();
        _loader->unload();
        return false;
    }
    
    // Create the game library instance
    auto createFunc = reinterpret_cast<create_game_t>(createSymbol);
    _currentGameLib = createFunc();
    if (!_currentGameLib) {
        _lastError = "Failed to create game library instance";
        _loader->unload();
        return false;
    }
    
    _currentGameLibPath = path;
    return true;
}

IGameLibrary* LibraryManager::getCurrentGameLibrary() const
{
    return _currentGameLib;
}

bool LibraryManager::switchToNextGameLibrary()
{
    if (_gameLibs.empty()) {
        _lastError = "No game libraries available";
        return false;
    }
    
    // Find the current library in the map
    auto it = _gameLibs.begin();
    if (!_currentGameLibPath.empty()) {
        for (; it != _gameLibs.end(); ++it) {
            if (it->second == _currentGameLibPath) {
                ++it;
                break;
            }
        }
        
        // If we reached the end, wrap around
        if (it == _gameLibs.end()) {
            it = _gameLibs.begin();
        }
    }
    
    // Load the next library
    return loadGameLibrary(it->first);
}

void LibraryManager::unloadCurrentGraphicsLibrary()
{
    if (_currentGraphicsLib) {
        // Get the destroy function
        if (_loader->isLoaded()) {
            void* destroySymbol = _loader->getSymbol("destroyGraphicsLibrary");
            if (destroySymbol) {
                auto destroyFunc = reinterpret_cast<destroy_graphics_t>(destroySymbol);
                destroyFunc(_currentGraphicsLib);
            }
        }
        
        _currentGraphicsLib = nullptr;
        _currentGraphicsLibPath = "";
        _loader->unload();
    }
}

void LibraryManager::unloadCurrentGameLibrary()
{
    if (_currentGameLib) {
        // Get the destroy function
        if (_loader->isLoaded()) {
            void* destroySymbol = _loader->getSymbol("destroyGameLibrary");
            if (destroySymbol) {
                auto destroyFunc = reinterpret_cast<destroy_game_t>(destroySymbol);
                destroyFunc(_currentGameLib);
            }
        }
        
        _currentGameLib = nullptr;
        _currentGameLibPath = "";
        _loader->unload();
    }
}

std::string LibraryManager::getLastError() const
{
    return _lastError;
}

} // namespace arcd
