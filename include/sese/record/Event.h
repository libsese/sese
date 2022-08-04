/**
 * @file Event.h
 * @brief 日志事件类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once

#include "sese/Config.h"
#include "sese/DateTime.h"
#include <memory>

#ifdef _WIN32
#pragma warning(disable : 4251)
#pragma warning(disable : 4819)
#endif
namespace sese::record {

    /// @brief 日志等级
    enum class Level {
        /// @brief 调试
        DEBUG = 0,
        /// @brief 信息
        INFO = 1,
        /// @brief 警告
        WARN = 2,
        /// @brief 错误
        ERR = 3
    };

    /**
     * @brief 日志事件类
     */
    class API Event {
    public:
        typedef std::shared_ptr<Event> Ptr;

    public:
        Event(DateTime::Ptr dateTime, Level lv, const char *threadName, tid_t id, const char *file, int32_t line,
              const char *msg, const char *tag = RECORD_DEFAULT_TAG) noexcept {
            this->dateTime = std::move(dateTime);
            this->level = lv;
            this->threadName = threadName;
            this->threadId = id;
            this->file = file;
            this->line = line;
            this->message = msg;
            this->tag = tag;
        }

        [[nodiscard]] const DateTime::Ptr &getTime() const noexcept { return this->dateTime; }
        [[nodiscard]] Level getLevel() const noexcept { return this->level; }
        [[nodiscard]] tid_t getThreadId() const noexcept { return this->threadId; }
        [[nodiscard]] int32_t getLine() const noexcept { return this->line; }
        [[nodiscard]] const char *getFileName() const noexcept { return this->file; }
        [[nodiscard]] const char *getMessage() const noexcept { return this->message; }
        [[nodiscard]] const char *getThreadName() const noexcept { return this->threadName; }
        [[nodiscard]] const char *getTag() const noexcept { return this->tag; }

    private:
        DateTime::Ptr dateTime;
        Level level;
        const char *threadName = nullptr;
        tid_t threadId;
        const char *file = nullptr;
        int32_t line;
        const char *message = nullptr;
        const char *tag = nullptr;
    };
}// namespace sese::record