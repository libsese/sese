/// \file BlockAppender.h
/// \author kaoru
/// \date 2022年8月6日
/// \brief 分块文件日志输出源类
/// \version 0.1

#pragma once
#include <sese/record/AbstractAppender.h>
#include <sese/FileStream.h>
#include <sese/BufferedStream.h>
#include <mutex>

namespace sese::record {

    /// 分块文件日志输出源类
    class API BlockAppender : public AbstractAppender {
    public:
        explicit BlockAppender(size_t blockMaxSize, Level level = Level::DEBUG);

        ~BlockAppender() noexcept;

        void dump(const char *buffer, size_t i) noexcept override;

    private:
        size_t size = 0;
        std::mutex mutex;
        size_t maxSize = 0;
        BufferedStream::Ptr bufferedStream;
        FileStream::Ptr fileStream;
    };

}// namespace sese::record