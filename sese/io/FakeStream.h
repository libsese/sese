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
 * @file FakeStream.h
 * @author kaoru
 * @brief Mock stream template
 * @version 0.1
 * @date September 13, 2023
 */

#pragma once

#include "sese/Config.h"
#include "sese/io/Closeable.h"
#include "sese/io/PeekableStream.h"
#include "sese/io/Stream.h"

#include <istream>
#include <ostream>

namespace sese::io {

// GCOVR_EXCL_START

/// \brief Mock stream
/// \tparam T Wrapper type
template<typename T>
class FakeStream : public Stream {
public:
    explicit FakeStream(T *t) : t(t) {}

    int64_t read(void *buffer, size_t length) override { return t->read(buffer, length); }
    int64_t write(const void *buffer, size_t length) override { return t->write(buffer, length); };

protected:
    T *t;
};

/// \brief Closable mock stream
/// \tparam T Wrapper type
template<typename T>
class ClosableFakeStream : public FakeStream<T>, public sese::io::Closeable {
public:
    explicit ClosableFakeStream(T *t) : FakeStream<T>(t) {}
    void close() override { this->t->close(); }
};

/// @brief Standard library input wrapper stream
class StdInputStreamWrapper final
    : public InputStream,
      public PeekableStream {
public:
    explicit StdInputStreamWrapper(std::istream &stream);

    int64_t read(void *buffer, size_t length) override;

    /// Peek a certain number of bytes
    /// \warning This function is not recommended unless you fully understand what you are doing
    /// \see sese::StdOutputStreamWrapper::write
    /// \param buffer Buffer
    /// \param length Buffer size
    /// \return Actual size read
    int64_t peek(void *buffer, size_t length) override;

    /// Step a certain number of bytes
    /// \warning This function is not recommended unless you fully understand what you are doing
    /// \see sese::StdOutputStreamWrapper::write
    /// \param length Step size
    /// \return Step size
    int64_t trunc(size_t length) override;

private:
    std::istream &stream;
};

/// @brief Standard library output wrapper stream
class StdOutputStreamWrapper final
    : public OutputStream {
public:
    explicit StdOutputStreamWrapper(std::ostream &stream);

    /// Write buffer to the standard library input stream
    /// @param buffer Buffer to write
    /// @param length Buffer size to write
    /// @warning Do not use this function unless you fully understand what you are doing. Ensure that the stream.tellp() you pass in is not -1, otherwise it will not return the actual number of bytes written correctly
    /// @return Actual number of bytes written
    int64_t write(const void *buffer, size_t length) override;

private:
    std::ostream &stream;
    std::streamoff latest;
};

// GCOVR_EXCL_STOP

} // namespace sese::io
