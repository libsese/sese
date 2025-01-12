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

#include "sese/system/Library.h"

using sese::system::Library;

class Library::Impl {
public:
    using Ptr = std::unique_ptr<Impl>;

    HMODULE module;

    static Ptr create(const std::string &name) noexcept {
        auto handle = LoadLibraryA(name.c_str());
        if (handle == nullptr) {
            return nullptr;
        }
        return std::make_unique<Impl>(handle);
    }

    explicit Impl(HMODULE module) noexcept {
        this->module = module;
    }

    const void *findFunctionByName(const std::string &name) const {
        return reinterpret_cast<const void *>(GetProcAddress(module, name.c_str()));
    }

    ~Impl() noexcept {
        FreeLibrary(module);
    }
};

Library::Ptr Library::create(const std::string &name) noexcept {
    auto impl = Impl::create(name);
    if (!impl) {
        return nullptr;
    }
    return MAKE_UNIQUE_PRIVATE(Library, std::move(impl));
}

Library::Ptr Library::createWithPath(const Path &path) noexcept {
    return create(path.getNativePath());
}

Library::~Library() noexcept {
}

Library::Library(std::unique_ptr<Impl> impl) noexcept
    : impl(std::move(impl)) {
}

const void *Library::findFunctionByName(const std::string &name) const {
    return impl->findFunctionByName(name);
}
