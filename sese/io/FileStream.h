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

/**
 * @brief 文件流类
 */
class API FileStream final : public Stream, public Closeable, public PeekableStream {
public:
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

    int64_t getSeek();
    int32_t setSeek(int64_t offset, int32_t whence);

    int32_t flush();

    [[nodiscard]] int32_t getFd() const;

private:
    FileStream() noexcept = default;
    FILE *file = nullptr;
};

using File = FileStream;
} // namespace sese::io