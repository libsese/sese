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
 * @file FileStream.h
 * @brief File stream class (wrapper)
 * @date April 11, 2022
 * @author kaoru
 */

#pragma once

#include "sese/io/Stream.h"
#include "sese/io/Closeable.h"
#include "sese/io/PeekableStream.h"
#include "sese/system/Path.h"
#include "sese/util/ErrorCode.h"
#include "sese/util/Result.h"

namespace sese::io {

/// Seek Base
enum class Seek {
    /// The current pointer
    CUR = SEEK_CUR,
    /// Start position pointer
    BEGIN = SEEK_SET,
    /// End position pointer
    END = SEEK_END
};

/**
 * @brief File stream class
 */
class FileStream final : public Stream, public Closeable, public PeekableStream {
public:
#ifdef SESE_PLATFORM_WINDOWS
    static constexpr auto B_READ_W = "rb,ccs=utf-8";         // NOLINT
    static constexpr auto B_WRITE_TRUNC_W = "wb,ccs=utf-8";  // NOLINT
    static constexpr auto B_WRITE_APPEND_W = "ab,ccs=utf-8"; // NOLINT
    static constexpr auto B_TRUNC_W = "wb+,ccs=utf-8";       // NOLINT
    static constexpr auto B_APPEND_W = "ab+,ccs=utf-8";      // NOLINT
    static constexpr auto T_READ_W = "rt,ccs=utf-8";         // NOLINT
    static constexpr auto T_WRITE_TRUNC_W = "wt,ccs=utf-8";  // NOLINT
    static constexpr auto T_WRITE_APPEND_W = "at,ccs=utf-8"; // NOLINT
    static constexpr auto T_TRUNC_W = "wt+,ccs=utf-8";       // NOLINT
    static constexpr auto T_APPEND_W = "at+,ccs=utf-8";      // NOLINT
#endif
    static constexpr auto B_READ = "rb";         // NOLINT
    static constexpr auto B_WRITE_TRUNC = "wb";  // NOLINT
    static constexpr auto B_WRITE_APPEND = "ab"; // NOLINT
    static constexpr auto B_TRUNC = "wb+";       // NOLINT
    static constexpr auto B_APPEND = "ab+";      // NOLINT
    static constexpr auto T_READ = "rt";         // NOLINT
    static constexpr auto T_WRITE_TRUNC = "wt";  // NOLINT
    static constexpr auto T_WRITE_APPEND = "at"; // NOLINT
    static constexpr auto T_TRUNC = "wt+";       // NOLINT
    static constexpr auto T_APPEND = "at+";      // NOLINT

    using Ptr = std::shared_ptr<FileStream>;

    /// Open a file stream
    /// \param file_path File path
    /// \param mode Open mode
    /// \retval nullptr Failed to open
    /// \retval File stream object
    static FileStream::Ptr create(const std::string &file_path, const char *mode) noexcept;

    /// Open a file in UNIX-LIKE style
    /// \param path UNIX-LIKE path
    /// \param mode Open mode
    /// \retval nullptr Failed to open
    static FileStream::Ptr createWithPath(const system::Path &path, const char *mode) noexcept;

    static Result<Ptr, ErrorCode> createEx(const std::string &file_path, const char *mode) noexcept;

    ~FileStream() override = default;

    int64_t read(void *buffer, size_t length) override;
    int64_t write(const void *buffer, size_t length) override;

    void close() override;

    int64_t peek(void *buffer, size_t length) override;
    int64_t trunc(size_t length) override;

    /// This function is used to determine if the end of the file has been reached
    /// \warning This requires an initial read failure to obtain a valid value
    /// \return Result
    [[nodiscard]] bool eof() const;

    [[nodiscard]] int64_t getSeek() const;
    [[nodiscard]] int32_t setSeek(int64_t offset, int32_t whence) const;
    [[nodiscard]] int32_t setSeek(int64_t offset, Seek type) const;

    [[nodiscard]] int32_t flush() const;

    [[nodiscard]] int32_t getFd() const;

private:
    FileStream() noexcept = default;
    FILE *file = nullptr;
};

using File = FileStream;
} // namespace sese::io