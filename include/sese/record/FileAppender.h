/**
 * @file FileAppender.h
 * @author kaoru
 * @brief 日志文件输出源类
 * @date 2022年3月28日
 */
#pragma once
#include <utility>

#include "sese/record/AbstractAppender.h"
#include "sese/FileStream.h"

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

namespace sese {

    /**
     * @brief 日志文件输出源类
     */
    class API FileAppender : public AbstractAppender {
    public:
        typedef std::shared_ptr<FileAppender> Ptr;

        explicit FileAppender(FileStream::Ptr fileStream, const AbstractFormatter::Ptr &formatter, Level level = Level::DEBUG)
            : AbstractAppender(formatter, level),
              fileStream(std::move(fileStream)) {
        }

        void dump(const Event::Ptr &event) noexcept override;

    private:
        std::shared_ptr<FileStream> fileStream;
    };

}// namespace sese