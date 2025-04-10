/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** ILibraryLoader
*/

#ifndef I_LIBRARY_LOADER_HPP
#define I_LIBRARY_LOADER_HPP

#include <string>
#include <memory>
#include <type_traits>

namespace arcd {
    class ILibraryLoader {
        public:
            virtual ~ILibraryLoader() = default;

            virtual bool load(const std::string& path) = 0;
            virtual void* getSymbol(const std::string& symbol) = 0;
            virtual bool unload() = 0;
            virtual std::string getError() const = 0;
            virtual bool isLoaded() const = 0;
            
            // Template method for type-safe symbol loading
            template<typename T>
            T getSymbolAs(const std::string& symbol) {
                static_assert(std::is_pointer<T>::value, "T must be a pointer type");
                return reinterpret_cast<T>(getSymbol(symbol));
            }
    };
}

#endif // I_LIBRARY_LOADER_HPP
