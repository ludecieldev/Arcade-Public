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
    /**
     * @brief Interface for dynamic library loading
     */
    class ILibraryLoader {
        public:
            /**
             * @brief Virtual destructor
             */
            virtual ~ILibraryLoader() = default;

            /**
             * @brief Load a dynamic library from a file path
             * @param path file path to the library
             * @return true if loading succeeded, false otherwise
             */
            virtual bool load(const std::string& path) = 0;
            
            /**
             * @brief Get a symbol from the loaded library
             * @param symbol name of the symbol to retrieve
             * @return pointer to the symbol, or nullptr if not found
             */
            virtual void* getSymbol(const std::string& symbol) = 0;
            
            /**
             * @brief Unload the currently loaded library
             * @return true if unloading succeeded, false otherwise
             */
            virtual bool unload() = 0;
            
            /**
             * @brief Get the last error message
             * @return string containing the error message
             */
            virtual std::string getError() const = 0;
            
            /**
             * @brief Check if a library is currently loaded
             * @return true if a library is loaded, false otherwise
             */
            virtual bool isLoaded() const = 0;
            
            /**
             * @brief Template method for type-safe symbol loading
             * @tparam T type of the symbol (must be a pointer type)
             * @param symbol name of the symbol to retrieve
             * @return typed pointer to the symbol
             */
            template<typename T>
            T getSymbolAs(const std::string& symbol) {
                static_assert(std::is_pointer<T>::value, "T must be a pointer type");
                return reinterpret_cast<T>(getSymbol(symbol));
            }
    };
}

#endif // I_LIBRARY_LOADER_HPP
