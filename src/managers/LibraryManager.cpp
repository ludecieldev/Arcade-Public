#include "managers/LibraryManager.hpp"
#include <iostream>
#include <filesystem>
#include <unordered_set>
#include <algorithm>
#include <sstream>
#include <fstream>

namespace arcd {

LibraryManager::LibraryManager(const std::string& libDirectory)
    : _libDirectory(libDirectory),
      _graphicsLoader(std::make_unique<DLLoader>()),
      _gameLoader(std::make_unique<DLLoader>()),
      _currentGraphicsIndex(0),
      _currentGameIndex(0)
{
    scanLibraries();
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
        if (!std::filesystem::exists(_libDirectory)) {
            std::cerr << "Library directory does not exist: " << _libDirectory << std::endl;
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(_libDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".so") {
                std::string path = entry.path().string();
                
                if (isGraphicsLibrary(path)) {
                    std::string name = entry.path().filename().string();
                    _graphicsLibs[name] = path;
                }
                else if (isGameLibrary(path)) {
                    std::string name = entry.path().filename().string();
                    _gameLibs[name] = path;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error scanning libraries: " << e.what() << std::endl;
    }
}

std::vector<std::string> LibraryManager::getGraphicsLibraries() const
{
    std::vector<std::string> result;
    for (const auto& [name, path] : _graphicsLibs) {
        result.push_back(path);
    }
    return result;
}

std::vector<std::string> LibraryManager::getGameLibraries() const
{
    std::vector<std::string> result;
    for (const auto& [name, path] : _gameLibs) {
        result.push_back(path);
    }
    return result;
}

bool LibraryManager::loadGraphicsLibrary(const std::string& path)
{
    // Unload current library if any
    unloadCurrentGraphicsLibrary();
    
    // Recréer le loader s'il n'existe pas
    if (!_graphicsLoader) {
        _graphicsLoader = std::make_unique<DLLoader>();
    }
    
    // Find the library path
    std::string libPath;
    if (_graphicsLibs.find(path) != _graphicsLibs.end()) {
        libPath = _graphicsLibs[path];
    } else {
        // If not found in the map, try to use the name as a path
        libPath = path;
    }
    
    // Load the library
    if (!_graphicsLoader->load(libPath)) {
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
    
    _currentGraphicsLibPath = libPath;
    return true;
}

bool LibraryManager::loadGameLibrary(const std::string& name)
{
    // Unload current library if any
    unloadCurrentGameLibrary();
    
    // Recréer le loader s'il n'existe pas
    if (!_gameLoader) {
        _gameLoader = std::make_unique<DLLoader>();
    }
    
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

void LibraryManager::unloadCurrentGraphicsLibrary()
{
    if (_currentGraphicsLib) {
        try {
            // Appeler cleanup avant de détruire
            _currentGraphicsLib->cleanup();
        } catch (const std::exception& e) {
            std::cerr << "Warning: Error during graphics library cleanup: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Warning: Unknown error during graphics library cleanup" << std::endl;
        }
    }
    
    _currentGraphicsLib.reset();
    
    if (_graphicsLoader) {
        _graphicsLoader->unload();
    }
    
    _currentGraphicsLibPath.clear();
}

void LibraryManager::unloadCurrentGameLibrary()
{
    if (_currentGameLib) {
        try {
            // Appeler cleanup avant de détruire
            _currentGameLib->cleanup();
        } catch (const std::exception& e) {
            std::cerr << "Warning: Error during game library cleanup: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Warning: Unknown error during game library cleanup" << std::endl;
        }
    }
    
    _currentGameLib.reset();
    
    if (_gameLoader) {
        _gameLoader->unload();
    }
    
    _currentGameLibPath.clear();
}

bool LibraryManager::loadNextGraphicsLibrary()
{
    auto graphicsLibs = getGraphicsLibraries();
    if (graphicsLibs.empty()) {
        _lastError = "No graphics libraries available";
        return false;
    }

    size_t nextIndex = (_currentGraphicsIndex + 1) % graphicsLibs.size();
    bool success = loadGraphicsLibrary(graphicsLibs[nextIndex]);
    if (success) {
        _currentGraphicsIndex = nextIndex;
    }
    return success;
}

bool LibraryManager::loadNextGameLibrary()
{
    auto gameLibs = getGameLibraries();
    if (gameLibs.empty()) {
        _lastError = "No game libraries available";
        return false;
    }

    size_t nextIndex = (_currentGameIndex + 1) % gameLibs.size();
    bool success = loadGameLibrary(gameLibs[nextIndex]);
    if (success) {
        _currentGameIndex = nextIndex;
    }
    return success;
}

IGraphicsLibrary& LibraryManager::getCurrentGraphicsLibrary()
{
    if (!_currentGraphicsLib) {
        throw std::runtime_error("No graphics library loaded");
    }
    return *_currentGraphicsLib;
}

IGameLibrary& LibraryManager::getCurrentGameLibrary()
{
    if (!_currentGameLib) {
        throw std::runtime_error("No game library loaded");
    }
    return *_currentGameLib;
}

bool LibraryManager::hasGraphicsLibrary() const
{
    return _currentGraphicsLib != nullptr;
}

bool LibraryManager::hasGameLibrary() const
{
    return _currentGameLib != nullptr;
}

std::string LibraryManager::getLastError() const
{
    return _lastError;
}

bool LibraryManager::isGraphicsLibrary(const std::string& path) const
{
    try {
        // Créer un DLLoader temporaire
        DLLoader loader;
        if (!loader.load(path)) {
            return false;
        }

        // Vérifier si les symboles nécessaires existent
        void* createSymbol = loader.getSymbol("createGraphicsLibrary");
        void* destroySymbol = loader.getSymbol("destroyGraphicsLibrary");
        
        return createSymbol != nullptr && destroySymbol != nullptr;
    }
    catch (...) {
        return false;
    }
}

bool LibraryManager::isGameLibrary(const std::string& path) const
{
    try {
        // Créer un DLLoader temporaire
        DLLoader loader;
        if (!loader.load(path)) {
            return false;
        }

        // Vérifier si les symboles nécessaires existent
        void* createSymbol = loader.getSymbol("createGameLibrary");
        void* destroySymbol = loader.getSymbol("destroyGameLibrary");
        
        return createSymbol != nullptr && destroySymbol != nullptr;
    }
    catch (...) {
        return false;
    }
}

} // namespace arcd 