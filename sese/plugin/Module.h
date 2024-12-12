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

/// \file Module.h
/// \author kaoru
/// \date March 21, 2023
/// \version 0.1.0
/// \brief Plugin module object, adapted to sese-plugin related interfaces

#pragma once

#include "sese/plugin/Marco.h"
#include "sese/system/LibraryLoader.h"

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::plugin {

/// Plugin module object
class Module {
public:
    using Ptr = std::unique_ptr<Module>;
    using LibraryObject = sese::system::LibraryObject;

    /// \brief Attempt to load a module from a dynamic library
    /// \param path Dynamic library path
    /// \retval nullptr Loading error
    static Module::Ptr open(const std::string &path) noexcept;

    static Result<Ptr, ErrorCode> openEx(const std::string &path) noexcept;

    /// \brief Attempt to load a module from a dynamic library
    /// \param path Dynamic library path
    /// \retval nullptr Loading error
    static Module::Ptr openWithPath(const system::Path &path) noexcept;

    /// \brief Get the module name
    /// \return Module name
    const char *getName() noexcept;

    /// \brief Get the module version
    /// \return Module version
    const char *getVersion() noexcept;

    /// \brief Get the module description
    /// \return Module description
    const char *getDescription() noexcept;

    /// \brief Construct an object using the class factory of the module
    /// \param id Object identifier
    /// \retval nullptr Instantiation error
    BaseClass::Ptr createClass(const std::string &id) noexcept;

    /// \brief Construct an object using the class factory of the module and cast to a specified type
    /// \tparam TYPE Specified type
    /// \param name Object identifier
    /// \retval nullptr Instantiation error or type error
    template<typename TYPE>
    std::shared_ptr<TYPE> createClassAs(const std::string &name) {
        auto p = createClass(name);
        if (p) {
            return std::reinterpret_pointer_cast<TYPE>(p);
        }
        return nullptr;
    }

    /// \brief Get the type information of registered classes in the module
    /// \return Type information
    const ClassFactory::RegisterInfoMapType &
    getRegisterClassInfo() noexcept;

private:
    Module() = default;

    LibraryObject::Ptr object = nullptr;

    ModuleInfo *info = nullptr;
    ClassFactory *factory = nullptr;
};
} // namespace sese::plugin