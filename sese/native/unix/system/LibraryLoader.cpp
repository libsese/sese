#include "sese/system/LibraryLoader.h"

#include <dlfcn.h>

using sese::system::LibraryObject;

LibraryObject::LibraryObject(Module module) noexcept {
    this->module = module;
}
const void *LibraryObject::findFunctionByName(const std::string &name) const {
    return dlsym(module, name.c_str());
}

LibraryObject::~LibraryObject() noexcept {
    dlclose(module);
}

LibraryObject::Ptr LibraryObject::create(const std::string &name) noexcept {
    auto handle = dlopen(name.c_str(), RTLD_LAZY);
    if (handle == nullptr) {
        return nullptr;
    } else {
        return MAKE_SHARED_PRIVATE(LibraryObject, handle);
    }
}

LibraryObject::Ptr LibraryObject::createWithPath(const system::Path &path) noexcept {
    return LibraryObject::create(path.getNativePath());
}