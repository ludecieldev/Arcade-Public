/*
** EPITECH PROJECT, 2025
** bs
** File description:
** DLLoader
*/

#ifndef DLLOADER_HPP_
    #define DLLOADER_HPP_

#include <memory>
#include <string>
#include <dlfcn.h>

template<typename T>
class DLLoader {
    public:
        DLLoader(const std::string& libPath);
        ~DLLoader();
        std::shared_ptr<T> getInstance();

    private:
        void* _handle;
};

#include "DLLoader.tpp"

#endif /* !DLLOADER_HPP_ */