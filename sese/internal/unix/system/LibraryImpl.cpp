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

#include <dlfcn.h>
#include <memory>

using sese::system::Library;

class Library::Impl {
public:
    using Ptr = std::unique_ptr<Impl>;

    void *module;

    explicit Impl(Module module) noexcept {
        this->module = module;
    }

    [[nodiscard]] const void *findFunctionByName(const std::string &name) const {
        return dlsym(module, name.c_str());
    }

    ~Impl() noexcept {
        dlclose(module);
    }

    static Impl::Ptr create(const std::string &name) noexcept {
        auto handle = dlopen(name.c_str(), RTLD_LAZY);
        if (handle == nullptr) {
            return nullptr;
        } else {
            return std::make_unique<Impl>(handle);
        }
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
