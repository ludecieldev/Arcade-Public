/*
** EPITECH PROJECT, 2025
** test_archi
** File description:
** DLLoader
*/

#ifndef DLLOADER_TPP_
    #define DLLOADER_TPP_

#include <dlfcn.h>
#include <stdexcept>
#include "DLLoader.hpp"

template<typename T>
DLLoader<T>::DLLoader(const std::string& libPath)
{
    _handle = dlopen(libPath.c_str(), RTLD_LAZY);
    if (!_handle)
        throw std::runtime_error(dlerror());
}

template<typename T>
DLLoader<T>::~DLLoader()
{
    if (_handle)
        dlclose(_handle);
}

template<typename T>
std::shared_ptr<T> DLLoader<T>::getInstance()
{
    std::shared_ptr<T> (*creator)() =
        reinterpret_cast<std::shared_ptr<T> (*)()>(dlsym(_handle, "create"));
    if (!creator)
        throw std::runtime_error(dlerror());
    return creator();
}

#endif /* !DLLOADER_TPP_ */