/**
 * @file Event.h
 * @brief 日志事件类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once

#include "sese/Config.h"
#include "sese/util/DateTime.h"
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
    Event(DateTime date_time, Level lv, const char *thread_name, tid_t id, const char *file, int32_t line,
          const char *msg) noexcept {
        this->dateTime = date_time;
        this->level = lv;
        this->threadName = thread_name;
        this->threadId = id;
        this->file = file;
        this->line = line;
        this->message = msg;
    }

    [[nodiscard]] const DateTime &getTime() const noexcept { return this->dateTime; }
    [[nodiscard]] Level getLevel() const noexcept { return this->level; }
    [[nodiscard]] tid_t getThreadId() const noexcept { return this->threadId; }
    [[nodiscard]] int32_t getLine() const noexcept { return this->line; }
    [[nodiscard]] const char *getFileName() const noexcept { return this->file; }
    [[nodiscard]] const char *getMessage() const noexcept { return this->message; }
    [[nodiscard]] const char *getThreadName() const noexcept { return this->threadName; }

private:
    DateTime dateTime;
    Level level;
    const char *threadName = nullptr;
    tid_t threadId;
    const char *file = nullptr;
    int32_t line;
    const char *message = nullptr;
};
} // namespace sese::record