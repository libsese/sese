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
 * \file Decompressor.h
 * \brief Decompressor class
 * \version 0.1
 * \date March 4, 2023
 * \author kaoru
 */

#pragma once

#include "sese/util/ZlibConfig.h"
#include "sese/io/OutputStream.h"

namespace sese {

/// Decompressor class
class Decompressor {
public:
    using OutputStream = sese::io::OutputStream;

    /// Create a decompressor
    /// \param type Compression format
    /// \param buffer_size Set internal buffer size
    explicit Decompressor(CompressionType type, size_t buffer_size = ZLIB_CHUNK_SIZE);

    virtual ~Decompressor();

    /// Set the buffer to be decompressed
    /// \param input Buffer to be decompressed
    /// \param input_size Size of this buffer
    void input(const void *input, unsigned int input_size);

    /// Perform decompression
    /// \param out Decompressed stream
    /// \retval Z_OK (0) Current buffer block decompressed successfully
    /// \retval Z_STREAM_ERROR (-2) Other errors
    /// \retval Z_BUF_ERROR (-5) Output stream capacity insufficient
    int inflate(OutputStream *out);

    /// Reset z_stream object
    int reset();

    /// Size of the currently processed input buffer
    /// \return Buffer size
    [[nodiscard]] size_t getTotalIn() const;

    /// Size of the currently processed output buffer
    /// \return Buffer size
    [[nodiscard]] size_t getTotalOut() const;

private:
    void *stream;
    /// Represents allocated buffer size
    size_t cap = 0;
    /// Represents filled buffer size
    size_t length = 0;
    /// Represents read portion of the buffer
    size_t read = 0;
    /// Internal buffer
    unsigned char *buffer;
};
} // namespace sese