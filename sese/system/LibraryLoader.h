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
 * @author kaoru
 * @brief 外部库加载器
 * @date 2022年4月22日
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
 * @brief 库对象
 */
class  LibraryObject {
public:
    using Ptr = std::shared_ptr<LibraryObject>;
#ifdef _WIN32
    using Module = HMODULE;
#else
    using Module = void *;
#endif
    /**
     * @brief 加载一个外部库
     * @param name 库名称
     * @return 库对象，打开失败返回 nullptr
     */
    static LibraryObject::Ptr create(const std::string &name) noexcept;

    /**
     * @brief 加载一个外部库
     * @param name 库名称
     * @return 结果
     */
    static Result<Ptr, ErrorCode> createEx(const std::string &name) noexcept;

    /**
     * @brief 加载一个外部库
     * @param path 库路径
     * @return 库对象，打开失败返回 nullptr
     */
    static LibraryObject::Ptr createWithPath(const system::Path &path) noexcept;

    ~LibraryObject() noexcept;
    /**
     * @brief 根据名称返回库中函数指针
     * @param name 函数名称
     * @return 函数指针，找不到为 nullptr
     */
    [[nodiscard]] const void *findFunctionByName(const std::string &name) const;

private:
    explicit LibraryObject(Module module) noexcept;

    Module module;
};

} // namespace sese::system