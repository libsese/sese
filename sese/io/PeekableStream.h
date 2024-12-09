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

/// \file PeekableStream.h
/// \author kaoru
/// \brief Peekable stream interface
/// \version 0.2
/// \date June 11, 2023

#pragma once

#include "sese/Config.h"

namespace sese::io {

// GCOVR_EXCL_START

/// Peekable stream interface
class PeekableStream {
public:
    virtual ~PeekableStream() = default;
    typedef std::shared_ptr<PeekableStream> Ptr;

    /// Read content without stepping
    /// \param buffer Buffer
    /// \param length Buffer size
    /// \return Actual size read
    virtual int64_t peek(void *buffer, size_t length) = 0;

    /// Step only, without reading content
    /// \param length Step size
    /// \return Actual step size
    virtual int64_t trunc(size_t length) = 0;
};

// GCOVR_EXCL_STOP

} // namespace sese::io