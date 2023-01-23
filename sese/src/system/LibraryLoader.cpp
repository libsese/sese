#include "sese/system/LibraryLoader.h"

using sese::LibraryLoader;
using sese::LibraryObject;

LibraryObject::LibraryObject(Module module) noexcept {
    this->module = module;
}

#ifdef _WIN32
const void *LibraryObject::findFunctionByName(const std::string &name) const {
    return (const void *) GetProcAddress(module, name.c_str());
}

LibraryObject::~LibraryObject() noexcept {
    FreeLibrary(module);
}

LibraryObject::Ptr LibraryLoader::open(const std::string &name) noexcept {
    auto handle = LoadLibraryA(name.c_str());
    if (handle == nullptr) {
        return nullptr;
    } else {
        return std::make_shared<LibraryObject>(handle);
    }
}
#else
#include <dlfcn.h>
const void *LibraryObject::findFunctionByName(const std::string &name) const {
    return dlsym(module, name.c_str());
}

LibraryObject::~LibraryObject() noexcept {
    dlclose(module);
}

LibraryObject::Ptr LibraryLoader::open(const std::string &name) noexcept {
    auto handle = dlopen(name.c_str(), RTLD_LAZY);
    if (handle == nullptr) {
        return nullptr;
    } else {
        return std::make_shared<LibraryObject>(handle);
    }
}
#endif