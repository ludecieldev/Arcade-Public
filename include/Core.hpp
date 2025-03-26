/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** Core
*/

#ifndef CORE_HPP
#define CORE_HPP

#include "managers/LibraryManager.hpp"
#include "managers/ScoreManager.hpp"
#include <string>
#include <memory>

namespace arcd {

enum class AppState {
    MENU,
    GAME,
    EXIT
};

class Core {
private:
    std::unique_ptr<LibraryManager> _libManager;
    std::unique_ptr<ScoreManager> _scoreManager;
    AppState _state;
    std::string _playerName;
    
    // Menu options
    int _selectedMenuOption;
    std::vector<std::string> _menuOptions;
    
    void initializeMenu();
    void handleMenuInput(int key);
    void renderMenu();
    
    void handleGameInput(int key);
    void renderGame();
    
    void displayError(const std::string& message);

public:
    Core(const std::string& initialGraphicsLib);
    ~Core();
    
    bool initialize();
    void run();
    void cleanup();
    
    // Getters
    AppState getState() const;
};

} // namespace arcd

#endif // CORE_HPP