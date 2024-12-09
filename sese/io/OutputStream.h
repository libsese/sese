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
 * \file OutputStream.h
 * \date November 14, 2022
 * \author kaoru
 * \brief Stream output interface class
 * \version 0.1
 */

#pragma once

#include "sese/Config.h"
#include "sese/text/String.h"

#include <span>

namespace sese::io {

// GCOVR_EXCL_START

/// \brief Stream output interface class
/// \details Supports writing to std::vector, std::array, std::span, etc.,
/// but be cautious about whether T is a POD type.
/// Writing non-POD types may lead to unexpected events
class OutputStream {
public:
    typedef std::shared_ptr<OutputStream> Ptr;

    virtual ~OutputStream() noexcept = default;

    virtual int64_t write(const void *buffer, size_t length) = 0;

    int64_t write(const std::string_view &buffer);

    int64_t write(const text::StringView &buffer);

    template<typename T>
    int64_t write(std::vector<T> &buffer) {
        return write(buffer.data(), buffer.size());
    }

    template<typename T, size_t N>
    int64_t write(std::array<T, N> &buffer) {
        return write(buffer.data(), N * sizeof(T));
    }
};

// GCOVR_EXCL_STOP

} // namespace sese::io