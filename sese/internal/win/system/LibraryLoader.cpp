// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sese/system/LibraryLoader.h"

using sese::system::LibraryObject;

LibraryObject::Ptr LibraryObject::create(const std::string &name) noexcept {
    auto handle = LoadLibraryA(name.c_str());
    if (handle == nullptr) {
        return nullptr;
    } else {
        return MAKE_SHARED_PRIVATE(LibraryObject, handle);
    }
}

LibraryObject::Ptr LibraryObject::createWithPath(const system::Path &path) noexcept {
    return LibraryObject::create(path.getNativePath());
}

LibraryObject::LibraryObject(Module module) noexcept {
    this->module = module;
}

const void *LibraryObject::findFunctionByName(const std::string &name) const {
    return reinterpret_cast<const void *>(GetProcAddress(module, name.c_str()));
}

LibraryObject::~LibraryObject() noexcept {
    FreeLibrary(module);
}