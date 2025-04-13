/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** DLLoader
*/  

#include "utils/DLLoader.hpp"
#include <iostream>

namespace arcd {

    /**
     * @brief Constructor for DLLoader
     */
    DLLoader::DLLoader() : _handle(nullptr) {}

    /**
     * @brief Destructor for DLLoader
     * Ensures any loaded library is properly unloaded
     */
    DLLoader::~DLLoader() {
        if (_handle) {
            unload();
        }
    }

    /**
     * @brief Load a dynamic library into memory
     * @param path path to the library file
     * @return true if loading was successful, false otherwise
     */
    bool DLLoader::load(const std::string& path) {
        unload();
        
        _handle = dlopen(path.c_str(), RTLD_LAZY);
        if (!_handle) {
            _error = dlerror();
            return false;
        }
        
        return true;
    }

    /**
     * @brief Get a symbol from the loaded library
     * @param symbolName name of the symbol to retrieve
     * @return pointer to the symbol, or nullptr if an error occurred
     */
    void* DLLoader::getSymbol(const std::string& symbolName) {
        if (!_handle) {
            _error = "No library loaded";
            return nullptr;
        }
        
        dlerror();
        
        void* symbol = dlsym(_handle, symbolName.c_str());
        const char* dlsymError = dlerror();
        if (dlsymError) {
            _error = dlsymError;
            return nullptr;
        }
        
        return symbol;
    }

    /**
     * @brief Unload the currently loaded library
     * @return true if unloading was successful or no library was loaded, false otherwise
     */
    bool DLLoader::unload() {
        if (!_handle) {
            return true;
        }

        if (dlclose(_handle) != 0) {
            _error = dlerror();
            return false;
        }

        _handle = nullptr;
        return true;
    }

    /**
     * @brief Get the last error message
     * @return string containing the error message
     */
    std::string DLLoader::getError() const {
        return _error;
    }

    /**
     * @brief Check if a library is currently loaded
     * @return true if a library is loaded, false otherwise
     */
    bool DLLoader::isLoaded() const {
        return _handle != nullptr;
    }

}