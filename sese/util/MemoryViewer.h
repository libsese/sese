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
 * \brief Memory viewer tool
 * \date September 13, 2023
 */

#pragma once

#include "sese/util/Endian.h"
#include "sese/util/NotInstantiable.h"
#include "sese/io/OutputStream.h"

namespace sese {

/// Memory viewer tool
class  MemoryViewer final : public NotInstantiable {
public:
    using OutputStream = sese::io::OutputStream;

    MemoryViewer() = delete;

    /// \brief Get memory view
    /// \param output Output stream
    /// \param position Memory address
    /// \param size Memory size
    /// \param is_cap Uppercase
    static void peer(OutputStream *output, void *position, size_t size, bool is_cap = true) noexcept;

    /// \brief Get 8-bit memory view
    /// \param output Output stream
    /// \param is_cap Uppercase
    /// \param position Memory address
    static void peer8(OutputStream *output, void *position, bool is_cap = true) noexcept;

    /// \brief Get 16-bit memory view
    /// \param output Output stream
    /// \param is_cap Uppercase
    /// \param position Memory address
    static void peer16(OutputStream *output, void *position, bool is_cap = true) noexcept;

    /// \brief Get 16-bit memory view
    /// \param output Output stream
    /// \param position Memory address
    /// \param type Endian format for output
    /// \param is_cap Uppercase
    static void peer16(OutputStream *output, void *position, EndianType type, bool is_cap = true) noexcept;

    /// \brief Get 32-bit memory view
    /// \param output Output stream
    /// \param position Memory address
    /// \param is_cap Uppercase
    static void peer32(OutputStream *output, void *position, bool is_cap = true) noexcept;

    /// \brief Get 32-bit memory view
    /// \param output Output stream
    /// \param position Memory address
    /// \param type Endian format for output
    /// \param is_cap Uppercase
    static void peer32(OutputStream *output, void *position, EndianType type, bool is_cap = true) noexcept;

    /// \brief Get 64-bit memory view
    /// \param output Output stream
    /// \param position Memory address
    /// \param is_cap Uppercase
    static void peer64(OutputStream *output, void *position, bool is_cap = true) noexcept;

    /// \brief Get 64-bit memory view
    /// \param output Output stream
    /// \param position Memory address
    /// \param type Endian format for output
    /// \param is_cap Uppercase
    static void peer64(OutputStream *output, void *position, EndianType type, bool is_cap = true) noexcept;

    static char toChar(unsigned char ch, bool is_cap) noexcept;
};


} // namespace sese