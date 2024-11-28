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
* @brief 文件流类（包装）
* @date 2022年4月11日
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

/// Seek 基址
enum class Seek {
    /// 当前指针
    CUR = SEEK_CUR,
    /// 起始位置指针
    BEGIN = SEEK_SET,
    /// 末尾位置指针
    END = SEEK_END
};

/**
 * @brief 文件流类
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

    /// 打开一个文件流
    /// \param file_path 文件路径
    /// \param mode 打开模式
    /// \retval nullptr 打开失败
    /// \retval 文件流对象
    static FileStream::Ptr create(const std::string &file_path, const char *mode) noexcept;

    /// 以 UNIX-LIKE 风格打开一个文件
    /// \param path UNIX-LIKE 路径
    /// \param mode 打开模式
    /// \retval nullptr 打开失败
    static FileStream::Ptr createWithPath(const system::Path &path, const char *mode) noexcept;

    static Result<Ptr, ErrorCode> createEx(const std::string &file_path, const char *mode) noexcept;

    ~FileStream() override = default;

    int64_t read(void *buffer, size_t length) override;
    int64_t write(const void *buffer, size_t length) override;

    void close() override;

    int64_t peek(void *buffer, size_t length) override;
    int64_t trunc(size_t length) override;

    /// 此函数用于判断是否已经读取到文件末尾
    /// @warning 这首先需要先触发一次读取失败才能获取有效值
    /// @return 结果s
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