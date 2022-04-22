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

namespace sese {

    /**
     * @brief 日志文件输出源类
     */
    class API FileAppender : public AbstractAppender {
    public:
        typedef std::shared_ptr<FileAppender> Ptr;

        FileAppender(const std::string &fileName, const AbstractFormatter::Ptr &formatter, Level level = Level::DEBUG)
            : AbstractAppender(formatter, level),
              fileStream(std::make_shared<FileStream>(fileName, TEXT_WRITE_CREATE_APPEND)){
        }

        void dump(const Event::Ptr &event) noexcept override;

    private:
        std::shared_ptr<FileStream> fileStream;
    };

}// namespace sese