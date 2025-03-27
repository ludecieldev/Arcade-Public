/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** ILibraryLoader
*/

#ifndef I_LIBRARY_LOADER_HPP
#define I_LIBRARY_LOADER_HPP

#include <string>

namespace arcd {
    class ILibraryLoader {
        public:
            virtual ~ILibraryLoader() = default;

            virtual bool load(const std::string& path) = 0;
            virtual void* getSymbol(const std::string& symbol) = 0;
            virtual bool unload() = 0;
            virtual std::string getError() const = 0;
            virtual bool isLoaded() const = 0;
    };
}

#endif // I_LIBRARY_LOADER_HPP
