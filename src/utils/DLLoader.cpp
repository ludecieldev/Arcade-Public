/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** DLLoader
*/  

#include "utils/DLLoader.hpp"

namespace arcd {

    DLLoader::DLLoader() : _handle(nullptr) {}

    DLLoader::~DLLoader() {
        if (_handle) {
            unload();
        }
    }

    bool DLLoader::load(const std::string& path) {
        if (_handle) {
            unload();
        }

        _handle = dlopen(path.c_str(), RTLD_LAZY);
        if (!_handle) {
            _lastError = dlerror();
            return false;
        }
        return true;
    }

    void* DLLoader::getSymbol(const std::string& symbol) {
        if (!_handle) {
            _lastError = "No library loaded";
            return nullptr;
        }

        void* sym = dlsym(_handle, symbol.c_str());
        if (!sym) {
            _lastError = dlerror();
        }
        return sym;
    }

    bool DLLoader::unload() {
        if (!_handle) {
            return true;
        }

        if (dlclose(_handle) != 0) {
            _lastError = dlerror();
            return false;
        }

        _handle = nullptr;
        return true;
    }

    std::string DLLoader::getError() const {
        return _lastError;
    }

    bool DLLoader::isLoaded() const {
        return _handle != nullptr;
    }

}