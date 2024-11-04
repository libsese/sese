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
 * @file Noncopyable.h
 * @author kaoru
 * @date 2022年3月28日
 * @brief 不可复制类
 */
#pragma once

// GCOVR_EXCL_START

namespace sese {

/**
 * @brief 不可复制类
 */
class Noncopyable {
public:
    Noncopyable() = default;
    virtual ~Noncopyable() = default;

    /// 删除拷贝相关构造函数
    Noncopyable(const Noncopyable &) = delete;
    /// 删除拷贝相关构造函数
    Noncopyable &operator=(const Noncopyable &) = delete;
};
} // namespace sese

// GCOVR_EXCL_STOP