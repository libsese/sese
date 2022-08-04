/**
 * @file FileAppender.h
 * @author kaoru
 * @brief 日志文件输出源类
 * @date 2022年3月28日
 */
#pragma once
#include "sese/record/AbstractAppender.h"
#include "sese/FileStream.h"

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

namespace sese::record {
    /**
     * @brief 日志文件输出源类
     * @todo 使用缓存减少 IO 操作
     */
    class API FileAppender : public sese::record::AbstractAppender {
    public:
        typedef std::shared_ptr<FileAppender> Ptr;

        explicit FileAppender(FileStream::Ptr fileStream, Level level = Level::DEBUG)
            : AbstractAppender(level),
              fileStream(std::move(fileStream)) {
        }

        void dump(const char *buffer, size_t size) noexcept override;

    private:
        std::shared_ptr<FileStream> fileStream;
    };
}// namespace sese::record