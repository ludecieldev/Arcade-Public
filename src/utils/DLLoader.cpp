/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** DLLoader
*/  

#include "utils/DLLoader.hpp"
#include <iostream>

namespace arcd {

    DLLoader::DLLoader() : _handle(nullptr) {}

    DLLoader::~DLLoader() {
        if (_handle) {
            unload();
        }
    }

    bool DLLoader::load(const std::string& path) {
        // Unload any previously loaded library
        unload();
        
        std::cerr << "DEBUG: DLLoader attempting to load: " << path << std::endl;
        
        // Try to open the library
        _handle = dlopen(path.c_str(), RTLD_LAZY);
        if (!_handle) {
            _error = dlerror();
            std::cerr << "DEBUG: DLLoader error: " << _error << std::endl;
            return false;
        }
        
        std::cerr << "DEBUG: DLLoader successfully loaded library" << std::endl;
        return true;
    }

    void* DLLoader::getSymbol(const std::string& symbolName) {
        if (!_handle) {
            _error = "No library loaded";
            std::cerr << "DEBUG: DLLoader error: " << _error << std::endl;
            return nullptr;
        }
        
        std::cerr << "DEBUG: DLLoader looking for symbol: " << symbolName << std::endl;
        
        // Clear any existing error
        dlerror();
        
        // Try to get the symbol
        void* symbol = dlsym(_handle, symbolName.c_str());
        const char* dlsymError = dlerror();
        if (dlsymError) {
            _error = dlsymError;
            std::cerr << "DEBUG: DLLoader symbol error: " << _error << std::endl;
            return nullptr;
        }
        
        std::cerr << "DEBUG: DLLoader found symbol successfully" << std::endl;
        return symbol;
    }

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

    std::string DLLoader::getError() const {
        return _error;
    }

    bool DLLoader::isLoaded() const {
        return _handle != nullptr;
    }

}