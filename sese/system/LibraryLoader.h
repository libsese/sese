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

/**
 * @file LibraryLoader.h
 * @brief External Library Loader
 * @author kaoru
 * @date April 22, 2022
 */

#pragma once

#include "sese/Config.h"
#include "sese/system/Path.h"
#include "sese/util/ErrorCode.h"
#include "sese/util/Result.h"

#ifdef _WIN32
#pragma warning(disable : 4624)
#endif

namespace sese::system {

/**
 * @brief Library Object
 */
class LibraryObject {
public:
    using Ptr = std::shared_ptr<LibraryObject>;
#ifdef _WIN32
    using Module = HMODULE;
#else
    using Module = void *;
#endif
    /**
     * @brief Load an external library
     * @param name Library name
     * @return Library object, returns nullptr if loading fails
     */
    static LibraryObject::Ptr create(const std::string &name) noexcept;

    /**
     * @brief Load an external library
     * @param name Library name
     * @return Result
     */
    static Result<Ptr, ErrorCode> createEx(const std::string &name) noexcept;

    /**
     * @brief Load an external library
     * @param path Library path
     * @return Library object, returns nullptr if loading fails
     */
    static LibraryObject::Ptr createWithPath(const system::Path &path) noexcept;

    ~LibraryObject() noexcept;
    /**
     * @brief Return a function pointer from the library by name
     * @param name Function name
     * @return Function pointer, returns nullptr if not found
     */
    [[nodiscard]] const void *findFunctionByName(const std::string &name) const;

private:
    explicit LibraryObject(Module module) noexcept;

    Module module;
};

} // namespace sese::system
