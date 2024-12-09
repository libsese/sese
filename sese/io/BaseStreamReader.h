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
 * @file BaseStreamReader.h
 * @author kaoru
 * @brief Output stream wrapper class
 * @date April 13, 2022
 */

#pragma once

#include "sese/io/ByteBuilder.h"
#include "sese/io/Stream.h"
#include "sese/util/Util.h"

#include <sstream>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::io {
/**
 * @brief Output stream wrapping class, UTF-8 under Windows requires special handling
 */
template<typename T = char>
class  BaseStreamReader {
public:
    using Ptr = std::shared_ptr<BaseStreamReader<T>>;

public:
    /**
     * @brief Constructor
     * @param source Stream to be wrapped
     */
    explicit BaseStreamReader(const Stream::Ptr &source) {
        this->sourceStream = source;
        this->bufferStream = std::make_unique<ByteBuilder>();
    }

    /**
     * @brief Read a character
     * @param ch Character container
     * @return Whether the read was successful
     */
    bool read(T &ch) {
        if (bufferStream->read(&ch, sizeof(T)) == 0) {
            /// Cache missed, trying to read from the source
            auto len = preRead();
            if (0 == len) {
                /// The source has been read
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Read a line
     * @return Returns the read string; a length of zero indicates completion
     */
    std::basic_string<T> readLine() {
        std::basic_stringstream<T> string;
        // auto canReadSize = bufferStream->getLength() - bufferStream->getCurrentReadPos();
        auto can_read_size = bufferStream->getReadableSize();
        if (can_read_size == 0) {
            can_read_size += preRead();
        }

        if (can_read_size > 0) {
            T ch;
            while (read(ch)) {
                if (ch == '\r' || ch == '\n') {
                    auto empty = string.rdbuf()->in_avail() == 0;
                    if (!empty) {
                        break;
                    } else {
                        continue;
                    }
                }
                string << ch;
            }
        }
        bufferStream->freeCapacity();
        return string.str();
    }

    /**
     * Used to cache ahead-of-time data
     * @return Cache
     */
    const ByteBuilder::Ptr &getBuffer() noexcept { return this->bufferStream; }

private:

    /**
     * @brief Cache ahead-of-time data
     * @details This method is used to cache the data of the source stream in advance.
     *          It is used to improve the performance of the readLine method.
     * @return The number of bytes cached
     */
    int64_t preRead() {
        char buffer[STREAM_BYTE_STREAM_SIZE_FACTOR];
        auto len = sourceStream->read(buffer, STRING_BUFFER_SIZE_FACTOR);
        if (0 < len) {
            bufferStream->write(buffer, len);
        }
        return len;
    }

private:
    Stream::Ptr sourceStream;
    ByteBuilder::Ptr bufferStream;
};

using StreamReader = BaseStreamReader<char>;
using WStreamReader = BaseStreamReader<wchar_t>;
} // namespace sese::io