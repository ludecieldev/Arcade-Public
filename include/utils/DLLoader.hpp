/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** DLLoader
*/

#ifndef DL_LOADER_HPP
#define DL_LOADER_HPP

#include "../interfaces/ILibraryLoader.hpp"
#include <dlfcn.h>
#include <string>

namespace arcd {

    /**
     * @brief Implementation of ILibraryLoader for dynamic library loading
     */
    class DLLoader : public ILibraryLoader {
        private:
            /**
             * @brief Handle to the loaded library
             */
            void* _handle;
            
            /**
             * @brief Last error message
             */
            std::string _error;

        public:
            /**
             * @brief Constructor for DLLoader
             */
            DLLoader();
            
            /**
             * @brief Destructor for DLLoader
             */
            ~DLLoader() override;

            /**
             * @brief Load a dynamic library from a file path
             * @param path file path to the library
             * @return true if loading succeeded, false otherwise
             */
            bool load(const std::string& path) override;
            
            /**
             * @brief Get a symbol from the loaded library
             * @param symbol name of the symbol to retrieve
             * @return pointer to the symbol, or nullptr if not found
             */
            void* getSymbol(const std::string& symbol) override;
            
            /**
             * @brief Unload the currently loaded library
             * @return true if unloading succeeded, false otherwise
             */
            bool unload() override;
            
            /**
             * @brief Get the last error message
             * @return string containing the error message
             */
            std::string getError() const override;
            
            /**
             * @brief Check if a library is currently loaded
             * @return true if a library is loaded, false otherwise
             */
            bool isLoaded() const override;
    };

}

#endif // DL_LOADER_HPP