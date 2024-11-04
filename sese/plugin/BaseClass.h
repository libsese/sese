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
 * @file BaseClass.h
 * @brief 模块基类
 * @author kaoru
 * @version 0.2
 */

#pragma once

#include <memory>

// GCOVR_EXCL_START

namespace sese::plugin {
/// 模块基类
class BaseClass : public std::enable_shared_from_this<BaseClass> {
public:
    using Ptr = std::shared_ptr<BaseClass>;

    virtual ~BaseClass() = default;
};
} // namespace sese::plugin

// GCOVR_EXCL_STOP