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
 * \file MessageDigest.h
 * \brief 信息摘要工具类
 * \author kaoru
 * \version 0.1
 * \date 2022.12.26
 */
#pragma once

#include "sese/io/InputStream.h"
#include "sese/util/NotInstantiable.h"

namespace sese {
/// 信息摘要工具类
class  MessageDigest final : public NotInstantiable {
public:
    using InputStream = sese::io::InputStream;

    enum Type {
        MD5,
        SHA1,
        SHA256
    };

    MessageDigest() = delete;

    /// 进行摘要
    /// \param type 摘要算法类型
    /// \param source 信息来源
    /// \param is_cap 字母是否大写
    /// \retval nullptr 摘要失败
    /// \return 摘要结果
    std::unique_ptr<char[]> static digest(Type type, const InputStream::Ptr &source, bool is_cap = false) noexcept;
    std::unique_ptr<char[]> static digest(Type type, InputStream *source, bool is_cap = false) noexcept;
};
} // namespace sese