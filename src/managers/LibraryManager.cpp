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
/**
 * @brief Constructor for LibraryManager
 * @param libDirectory Directory where libraries are located
 */
LibraryManager::LibraryManager(const std::string& libDirectory)
    : _libDirectory(libDirectory), _currentGraphicsIndex(0), _currentGameIndex(0)
{
    _graphicsLoader = std::make_unique<DLLoader>();
    _gameLoader = std::make_unique<DLLoader>();
}

/**
 * @brief Constructor for LibraryManager
 * @param libDirectory Directory where libraries are located
 */
LibraryManager::~LibraryManager()
{
    unloadCurrentGraphicsLibrary();
    unloadCurrentGameLibrary();
}

/**
 * @brief Scans the library directory for available libraries
 */
void LibraryManager::scanLibraries()
{
    _graphicsLibs.clear();
    _gameLibs.clear();
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(_libDirectory)) {
            if (entry.path().extension() == ".so") {
                std::string path = entry.path().string();
                std::string name = entry.path().filename().string();
                
                if (_graphicsLoader->load(path)) {
                    void* createGraphicsSymbol = _graphicsLoader->getSymbol("createGraphicsLibrary");
                    if (createGraphicsSymbol) {
                        _graphicsLibs[name] = path;
                    }
                    _graphicsLoader->unload();
                }
                
                if (_gameLoader->load(path)) {
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

/**
 * @brief Returns the list of available graphics libraries
 * @return List of graphics library names
 */
std::vector<std::string> LibraryManager::getGraphicsLibraries() const
{
    std::vector<std::string> libs;
    for (const auto& [name, path] : _graphicsLibs) {
        libs.push_back(name);
    }
    return libs;
}

/**
 * @brief Returns the list of available game libraries
 * @return List of game library names
 */
std::vector<std::string> LibraryManager::getGameLibraries() const
{
    std::vector<std::string> libs;
    for (const auto& [name, path] : _gameLibs) {
        libs.push_back(name);
    }
    return libs;
}

/**
 * @brief Loads a graphics library by name
 * @param name Name of the graphics library to load
 * @return True if successful, false otherwise
 */
bool LibraryManager::loadGraphicsLibrary(const std::string& name)
{
    unloadCurrentGraphicsLibrary();
    
    std::string path;
    if (_graphicsLibs.find(name) != _graphicsLibs.end()) {
        path = _graphicsLibs[name];
    } else {
        path = name;
    }
    
    if (!_graphicsLoader->load(path)) {
        _lastError = "Failed to load graphics library: " + _graphicsLoader->getError();
        return false;
    }
    
    auto createFunc = _graphicsLoader->getSymbolAs<create_graphics_t>("createGraphicsLibrary");
    if (!createFunc) {
        _lastError = "Invalid graphics library: " + _graphicsLoader->getError();
        _graphicsLoader->unload();
        return false;
    }
    
    _currentGraphicsLib = createFunc();
    if (!_currentGraphicsLib) {
        _lastError = "Failed to create graphics library instance";
        _graphicsLoader->unload();
        return false;
    }
    
    if (!_currentGraphicsLib->initialize()) {
        _lastError = "Failed to initialize graphics library";
        unloadCurrentGraphicsLibrary();
        return false;
    }
    
    _currentGraphicsLibPath = path;
    return true;
}

/**
 * @brief Returns the currently loaded graphics library
 * @return Reference to the current graphics library
 */
IGraphicsLibrary& LibraryManager::getCurrentGraphicsLibrary()
{
    if (!_currentGraphicsLib || !_graphicsLoader->isLoaded()) {
        throw std::runtime_error("No graphics library loaded");
    }
    
    return *_currentGraphicsLib;
}

/**
 * @brief Checks if a graphics library is currently loaded
 * @return True if a graphics library is loaded, false otherwise
 */
bool LibraryManager::hasGraphicsLibrary() const
{
    return _currentGraphicsLib != nullptr && _graphicsLoader->isLoaded();
}

/**
 * @brief Loads the next graphics library in the list
 * @return True if successful, false otherwise
 */
bool LibraryManager::loadNextGraphicsLibrary()
{
    if (_graphicsLibs.empty()) {
        _lastError = "No graphics libraries available";
        return false;
    }

    _currentGraphicsIndex = (_currentGraphicsIndex + 1) % _graphicsLibs.size();
    
    auto it = _graphicsLibs.begin();
    std::advance(it, _currentGraphicsIndex);
    return loadGraphicsLibrary(it->first);
}

/**
 * @brief Loads a game library by name
 * @param name Name of the game library to load
 * @return True if successful, false otherwise
 */
bool LibraryManager::loadGameLibrary(const std::string& name)
{
    unloadCurrentGameLibrary();
    
    std::string path;
    if (_gameLibs.find(name) != _gameLibs.end()) {
        path = _gameLibs[name];
    } else {
        path = name;
    }
    
    if (!_gameLoader->load(path)) {
        _lastError = "Failed to load game library: " + _gameLoader->getError();
        return false;
    }
    
    auto createFunc = _gameLoader->getSymbolAs<create_game_t>("createGameLibrary");
    if (!createFunc) {
        _lastError = "Invalid game library: " + _gameLoader->getError();
        _gameLoader->unload();
        return false;
    }
    
    _currentGameLib = createFunc();
    if (!_currentGameLib) {
        _lastError = "Failed to create game library instance";
        _gameLoader->unload();
        return false;
    }
    
    _currentGameLibPath = path;
    return true;
}

/**
 * @brief Returns the currently loaded game library
 * @return Reference to the current game library
 */
IGameLibrary& LibraryManager::getCurrentGameLibrary()
{
    if (!_currentGameLib || !_gameLoader->isLoaded()) {
        throw std::runtime_error("No game library loaded");
    }
    
    return *_currentGameLib;
}

/**
 * @brief Checks if a game library is currently loaded
 * @return True if a game library is loaded, false otherwise
 */
bool LibraryManager::hasGameLibrary() const
{
    return _currentGameLib != nullptr && _gameLoader->isLoaded();
}

/**
 * @brief Loads the next game library in the list
 * @return True if successful, false otherwise
 */
bool LibraryManager::loadNextGameLibrary()
{
    if (_gameLibs.empty()) {
        _lastError = "No game libraries available";
        return false;
    }

    _currentGameIndex = (_currentGameIndex + 1) % _gameLibs.size();
    
    auto it = _gameLibs.begin();
    std::advance(it, _currentGameIndex);
    return loadGameLibrary(it->first);
}

/**
 * @brief Unloads the currently loaded graphics library
 */
void LibraryManager::unloadCurrentGraphicsLibrary()
{
    if (_currentGraphicsLib) {
        try {
            _currentGraphicsLib->cleanup();
        } catch (const std::exception& e) {
            std::cerr << "Warning: Error during graphics library cleanup: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Warning: Unknown error during graphics library cleanup" << std::endl;
        }
        
        _currentGraphicsLib.reset();
        _currentGraphicsLibPath = "";
        
        if (_graphicsLoader && _graphicsLoader->isLoaded()) {
            _graphicsLoader->unload();
        }
    }
}

/**
 * @brief Unloads the currently loaded game library
 */
void LibraryManager::unloadCurrentGameLibrary()
{
    if (_currentGameLib) {
        try {
            _currentGameLib->cleanup();
        } catch (const std::exception& e) {
            std::cerr << "Warning: Error during game library cleanup: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Warning: Unknown error during game library cleanup" << std::endl;
        }
        
        _currentGameLib.reset();
        _currentGameLibPath = "";
        
        if (_gameLoader && _gameLoader->isLoaded()) {
            _gameLoader->unload();
        }
    }
}

/**
 * @brief Returns the last error message
 * @return Last error message
 */
std::string LibraryManager::getLastError() const
{
    return _lastError;
}

} // namespace arcd
