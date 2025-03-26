/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** Main entry point
*/

#include <iostream>
#include <string>
#include "ArcadeCore.hpp"

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <graphics_library.so>" << std::endl;
        return 84;
    }

    try {
        arcd::ArcadeCore core(argv[1]); // segfault ici
        core.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }

    return 0;
}
