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
 * \brief Message digest utility class
 * \author kaoru
 * \version 0.1
 * \date December 26, 2022
 */

#pragma once

#include "sese/io/InputStream.h"
#include "sese/util/NotInstantiable.h"

namespace sese {
/// Message digest utility class
class MessageDigest final : public NotInstantiable {
public:
    using InputStream = sese::io::InputStream;

    enum Type {
        MD5,
        SHA1,
        SHA256
    };

    MessageDigest() = delete;

    /// Perform digest
    /// \param type Digest algorithm type
    /// \param source Source of information
    /// \param is_cap Whether letters are uppercase
    /// \retval nullptr Digest failed
    /// \return Digest result
    std::unique_ptr<char[]> static digest(Type type, const InputStream::Ptr &source, bool is_cap = false) noexcept;
    std::unique_ptr<char[]> static digest(Type type, InputStream *source, bool is_cap = false) noexcept;
};
} // namespace sese