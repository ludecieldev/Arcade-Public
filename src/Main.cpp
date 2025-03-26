/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** Main
*/

#include "Core.hpp"
#include <iostream>

int main(int argc, char** argv) {
    // Check arguments
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <graphics_library.so>" << std::endl;
        return 84;
    }
    
    try {
        // Create and run the core application
        arcd::Core arcade(argv[1]);
        
        if (!arcade.initialize()) {
            return 84;
        }
        
        arcade.run();
        arcade.cleanup();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
}