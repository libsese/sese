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

#ifdef SESE_PLATFORM_WINDOWS
#define BINARY_READ_EXISTED_W "rb,ccs=utf-8"
#define BINARY_WRITE_CREATE_TRUNC_W "wb,ccs=utf-8"
#define BINARY_WRITE_CREATE_APPEND_W "ab,ccs=utf-8"
#define BINARY_BOTH_CREATE_TRUNC_W "wb+,ccs=utf-8"
#define BINARY_BOTH_CREATE_APPEND_W "ab+,ccs=utf-8"
#define TEXT_READ_EXISTED_W "rt,ccs=utf-8"
#define TEXT_WRITE_CREATE_TRUNC_W "wt,ccs=utf-8"
#define TEXT_WRITE_CREATE_APPEND_W "at,ccs=utf-8"
#define TEXT_BOTH_CREATE_TRUNC_W "wt+,ccs=utf-8"
#define TEXT_BOTH_CREATE_APPEND_W "at+,ccs=utf-8"
#endif

#define BINARY_READ_EXISTED "rb"
#define BINARY_WRITE_CREATE_TRUNC "wb"
#define BINARY_WRITE_CREATE_APPEND "ab"
#define BINARY_BOTH_CREATE_TRUNC "wb+"
#define BINARY_BOTH_CREATE_APPEND "ab+"
#define TEXT_READ_EXISTED "rt"
#define TEXT_WRITE_CREATE_TRUNC "wt"
#define TEXT_WRITE_CREATE_APPEND "at"
#define TEXT_BOTH_CREATE_TRUNC "wt+"
#define TEXT_BOTH_CREATE_APPEND "at+"

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
class API FileStream final : public Stream, public Closeable, public PeekableStream {
public:
#ifdef SESE_PLATFORM_WINDOWS
    static constexpr const char *B_READ_W = BINARY_READ_EXISTED_W;               // NOLINT
    static constexpr const char *B_WRITE_TRUNC_W = BINARY_WRITE_CREATE_TRUNC_W;  // NOLINT
    static constexpr const char *B_WRITE_APPEND_W = BINARY_WRITE_CREATE_TRUNC_W; // NOLINT
    static constexpr const char *B_APPEND_W = BINARY_BOTH_CREATE_APPEND_W;       // NOLINT
    static constexpr const char *B_TRUNC_W = BINARY_BOTH_CREATE_TRUNC_W;         // NOLINT
    static constexpr const char *T_READ_W = TEXT_READ_EXISTED_W;                 // NOLINT
    static constexpr const char *T_WRITE_TRUNC_W = TEXT_WRITE_CREATE_TRUNC_W;    // NOLINT
    static constexpr const char *T_WRITE_APPEND_W = TEXT_WRITE_CREATE_TRUNC_W;   // NOLINT
    static constexpr const char *T_APPEND_W = TEXT_BOTH_CREATE_APPEND_W;         // NOLINT
    static constexpr const char *T_TRUNC_W = TEXT_BOTH_CREATE_TRUNC_W;           // NOLINT
#endif
    static constexpr const char *B_READ = BINARY_READ_EXISTED;               // NOLINT
    static constexpr const char *B_WRITE_TRUNC = BINARY_WRITE_CREATE_TRUNC;  // NOLINT
    static constexpr const char *B_WRITE_APPEND = BINARY_WRITE_CREATE_TRUNC; // NOLINT
    static constexpr const char *B_APPEND = BINARY_BOTH_CREATE_APPEND;       // NOLINT
    static constexpr const char *B_TRUNC = BINARY_BOTH_CREATE_TRUNC;         // NOLINT
    static constexpr const char *T_READ = TEXT_READ_EXISTED;                 // NOLINT
    static constexpr const char *T_WRITE_TRUNC = TEXT_WRITE_CREATE_TRUNC;    // NOLINT
    static constexpr const char *T_WRITE_APPEND = TEXT_WRITE_CREATE_TRUNC;   // NOLINT
    static constexpr const char *T_APPEND = TEXT_BOTH_CREATE_APPEND;         // NOLINT
    static constexpr const char *T_TRUNC = TEXT_BOTH_CREATE_TRUNC;           // NOLINT


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

    ~FileStream() override = default;

    int64_t read(void *buffer, size_t length) override;
    int64_t write(const void *buffer, size_t length) override;

    void close() override;

    int64_t peek(void *buffer, size_t length) override;
    int64_t trunc(size_t length) override;

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