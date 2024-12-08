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
 * \file Compressor.h
 * \brief Compressor class
 * \version 0.1
 * \date March 4, 2023
 * \author kaoru
 */

#pragma once

#include "sese/convert/ZlibConfig.h"
#include "sese/io/OutputStream.h"

namespace sese {

/// Compressor class
class Compressor {
public:
    using OutputStream = sese::io::OutputStream;

    /// Create a compressor
    /// \param type Compression format - GZIP option is not recommended here
    /// \param level Compression level
    /// \param buffer_size Set internal buffer size
    explicit Compressor(CompressionType type, size_t level, size_t buffer_size = ZLIB_CHUNK_SIZE);

    virtual ~Compressor();

    /// Set the buffer to be compressed
    /// \param input Buffer to be compressed
    /// \param input_size Size of this buffer
    void input(const void *input, unsigned int input_size);

    /// Perform compression
    /// \param out Output stream for compressed data
    /// \retval Z_OK (0) Current buffer block compressed successfully
    /// \retval Z_STREAM_ERROR (-2) Other errors
    /// \retval Z_BUF_ERROR (-5) Output stream capacity insufficient
    int deflate(OutputStream *out);

    /// Reset z_stream object
    /// \return Result
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