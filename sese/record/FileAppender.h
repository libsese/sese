/**
 * @file FileAppender.h
 * @author kaoru
 * @brief 日志文件输出源类
 * @date 2022年3月28日
 */
#pragma once

#include "sese/record/AbstractAppender.h"
#include "sese/io/FileStream.h"
#include "sese/io/BufferedStream.h"

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

namespace sese::record {
/**
 * @brief 日志文件输出源类
 */
class API FileAppender final : public sese::record::AbstractAppender {
public:
    typedef std::shared_ptr<FileAppender> Ptr;

    explicit FileAppender(io::FileStream::Ptr fileStream, Level level = Level::DEBUG);

    ~FileAppender() noexcept override;

    void dump(const char *buffer, size_t size) noexcept override;

private:
    io::BufferedStream::Ptr bufferedStream;
    io::FileStream::Ptr fileStream;
};
} // namespace sese::record