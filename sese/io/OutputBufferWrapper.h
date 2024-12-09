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
 * \file OutputBufferWrapper.h
 * \author kaoru
 * \date December 14, 2022
 * \version 0.1
 * \brief Output buffer wrapper
 */

#pragma once

#include "sese/io/OutputStream.h"

#include <cstdint>

namespace sese::io {

// GCOVR_EXCL_START

/// \brief Output buffer wrapper
class  OutputBufferWrapper final : public OutputStream {
public:
    OutputBufferWrapper(char *buffer, size_t cap);

    int64_t write(const void *buffer, size_t length) override;

    void reset() noexcept;

    [[nodiscard]] const char *getBuffer() const;
    [[nodiscard]] size_t getLength() const;
    [[nodiscard]] size_t getCapacity() const;

protected:
    char *buffer = nullptr;
    size_t len = 0;
    size_t cap = 0;
};

// GCOVR_EXCL_STOP

} // namespace sese::io