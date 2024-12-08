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
 * @file ClassFactory.h
 * @brief Class factory built-in implementation
 * @author kaoru
 * @version 1.0
 */

#pragma once

#include "sese/plugin/BaseClass.h"
#include "sese/util/ErrorCode.h"
#include "sese/util/Result.h"

#include <map>
#include <string>
#include <typeinfo>
#include <functional>

namespace sese::plugin {

// GCOVR_EXCL_START

/// Class factory built-in implementation
class ClassFactory {
public:
    /// Registration Information
    struct RegisterInfo {
        const std::type_info *info;
        std::function<std::shared_ptr<BaseClass>()> builder;
    };
    using RegisterInfoMapType = std::map<std::string, RegisterInfo>;

    ClassFactory() noexcept = default;
    ClassFactory(ClassFactory &&class_factory) = delete;
    ClassFactory(const ClassFactory &class_factory) = delete;
    virtual ~ClassFactory() = default;

    virtual void init() = 0;

    /// \brief Create an instance of a registered class
    /// \param id Class registration name
    /// \retval nullptr Class not found
    /// \return Class instance
    BaseClass::Ptr createClassWithName(const std::string &id) noexcept;

    Result<BaseClass::Ptr, ErrorCode> createClassWithNameEx(const std::string &id) noexcept;

    /// \brief Get the type information of a registered class
    /// \return Class information map
    const RegisterInfoMapType &getRegisterClassInfo() noexcept;

protected:
    RegisterInfoMapType infoMap{};
};

// GCOVR_EXCL_STOP

} // namespace sese::plugin