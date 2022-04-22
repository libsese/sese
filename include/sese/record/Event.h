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
#endif
namespace sese {

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
        Event(const DateTime::Ptr &dateTime, Level lv, const char *threadName, pid_t id, const char *file, int32_t line,
              const char *msg, const char *filter = RECORD_DEFAULT_FILTER) noexcept {
            this->dateTime = dateTime;
            this->level = lv;
            this->threadName = threadName;
            this->threadId = id;
            this->file = file;
            this->line = line;
            this->message = msg;
            this->filter = filter;
        }

        [[nodiscard]] DateTime::Ptr getTime() const noexcept { return this->dateTime; }
        [[nodiscard]] Level getLevel() const noexcept { return this->level; }
        [[nodiscard]] pid_t getThreadId() const noexcept { return this->threadId; }
        [[nodiscard]] int32_t getLine() const noexcept { return this->line; }
        [[nodiscard]] const char *getFileName() const noexcept { return this->file; }
        [[nodiscard]] const char *getMessage() const noexcept { return this->message; }
        [[nodiscard]] const char *getThreadName() const noexcept { return this->threadName; }
        [[nodiscard]] const char *getLoggerName() const noexcept { return this->logName; }
        [[nodiscard]] const char *getFilter() const noexcept { return this->filter; }

        void setLogName(const char *name) noexcept { this->logName = name; }

    private:
        DateTime::Ptr dateTime;
        Level level;
        const char *threadName = nullptr;
        pid_t threadId;
        const char *file = nullptr;
        int32_t line;
        const char *message = nullptr;
        const char *logName = nullptr;
        const char *filter = nullptr;
    };
}// namespace sese