/**
 * @file FileAppender.h
 * @author kaoru
 * @brief 日志文件输出源类
 * @date 2022年3月28日
 */
#pragma once
#include "AbstractAppender.h"
#include <cstdlib>

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

        FileAppender(const std::string &fileName, const AbstractFormatter::Ptr &formatter, Level level = Level::DEBUG) : AbstractAppender(formatter, level) {
            this->fileName = fileName;
            this->file = fopen(fileName.c_str(), "a+");
        }

        void dump(const Event::Ptr &event) noexcept override;

        ~FileAppender();

        [[nodiscard]] const std::string &getFileName() const noexcept { return this->fileName; }

    private:
        std::string fileName;
        FILE *file = nullptr;
    };

}// namespace sese