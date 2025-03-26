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

namespace arcd {

class DLLoader : public ILibraryLoader {
private:
    void* _handle;
    std::string _lastError;

public:
    DLLoader();
    ~DLLoader() override;

    bool load(const std::string& path) override;
    void* getSymbol(const std::string& symbol) override;
    bool unload() override;
    std::string getError() const override;
    bool isLoaded() const override;
};

}

#endif // DL_LOADER_HPP