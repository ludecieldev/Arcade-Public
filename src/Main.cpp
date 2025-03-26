/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** Main
*/

#include "Core.hpp"
#include "utils/Error.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    // Check for correct number of arguments
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <graphics_library>" << std::endl;
        return 84;
    }
    
    try {
        // Initialize the core with the specified graphics library
        arcd::Core core(argv[1]);
        
        // Initialize and run the core
        if (!core.initialize()) {
            return 84;
        }
        
        core.run();
        return 0;
    } catch (const arcd::ArcadeError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 84;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 84;
    }
}