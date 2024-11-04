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
 * \file MemoryViewer.h
 * \author kaoru
 * \version 0.1
 * \brief 内存视图工具
 * \date 2023年9月13日
 */

#pragma once

#include "sese/util/Endian.h"
#include "sese/util/NotInstantiable.h"
#include "sese/io/OutputStream.h"

namespace sese {

/// 内存视图工具
class  MemoryViewer final : public NotInstantiable {
public:
    /// 工具所使用输出流
    using OutputStream = sese::io::OutputStream;

    MemoryViewer() = delete;

    /// \brief 获取内存视图
    /// \param output 输出流
    /// \param position 内存地址
    /// \param size 内存大小
    /// \param is_cap 是否大写
    static void peer(OutputStream *output, void *position, size_t size, bool is_cap = true) noexcept;

    /// \brief 获取 8 bits 内存视图
    /// \param output 输出流
    /// \param is_cap 是否大写
    /// \param position 内存地址
    static void peer8(OutputStream *output, void *position, bool is_cap = true) noexcept;

    /// \brief 获取 16 bits 内存视图
    /// \param output 输出流
    /// \param is_cap 是否大写
    /// \param position 内存地址
    static void peer16(OutputStream *output, void *position, bool is_cap = true) noexcept;

    /// \brief 获取 16 bits 内存视图
    /// \param output 输出流
    /// \param position 内存地址
    /// \param type 输出时的 endian 格式
    /// \param is_cap 是否大写
    static void peer16(OutputStream *output, void *position, EndianType type, bool is_cap = true) noexcept;

    /// \brief 获取 32 bits 内存视图
    /// \param output 输出流
    /// \param position 内存地址
    /// \param is_cap 是否大写
    static void peer32(OutputStream *output, void *position, bool is_cap = true) noexcept;

    /// \brief 获取 32 bits 内存视图
    /// \param output 输出流
    /// \param position 内存地址
    /// \param type 输出时的 endian 格式
    /// \param is_cap 是否大写
    static void peer32(OutputStream *output, void *position, EndianType type, bool is_cap = true) noexcept;

    /// \brief 获取 64 bits 内存视图
    /// \param output 输出流
    /// \param position 内存地址
    /// \param is_cap 是否大写
    static void peer64(OutputStream *output, void *position, bool is_cap = true) noexcept;

    /// \brief 获取 64 bits 内存视图
    /// \param output 输出流
    /// \param position 内存地址
    /// \param type 输出时的 endian 格式
    /// \param is_cap 是否大写
    static void peer64(OutputStream *output, void *position, EndianType type, bool is_cap = true) noexcept;

    static char toChar(unsigned char ch, bool is_cap) noexcept;
};


} // namespace sese