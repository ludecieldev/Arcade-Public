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

namespace arcd {

class LibraryManager {
private:
    std::string _libDirectory;
    std::unique_ptr<ILibraryLoader> _loader;
    std::string _lastError;
    
    std::map<std::string, std::string> _graphicsLibs;
    std::map<std::string, std::string> _gameLibs;
    
    IGraphicsLibrary* _currentGraphicsLib;
    std::string _currentGraphicsLibPath;
    
    IGameLibrary* _currentGameLib;
    std::string _currentGameLibPath;

public:
    LibraryManager(const std::string& libDirectory);
    ~LibraryManager();
    
    // Library discovery
    void scanLibraries();
    std::vector<std::string> getGraphicsLibraries() const;
    std::vector<std::string> getGameLibraries() const;
    
    // Graphics library management
    bool loadGraphicsLibrary(const std::string& name);
    IGraphicsLibrary* getCurrentGraphicsLibrary() const;
    bool switchToNextGraphicsLibrary();
    
    // Game library management
    bool loadGameLibrary(const std::string& name);
    IGameLibrary* getCurrentGameLibrary() const;
    bool switchToNextGameLibrary();
    
    // Cleanup
    void unloadCurrentGraphicsLibrary();
    void unloadCurrentGameLibrary();
    
    // Error handling
    std::string getLastError() const;
};

}

#endif // LIBRARY_MANAGER_HPP