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
#include <utility>

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
class  Event {
public:
    typedef std::shared_ptr<Event> Ptr;

public:
    Event(DateTime date_time, Level lv, std::string thread_name, tid_t id, std::string file, int32_t line,
          std::string msg) noexcept {
        this->dateTime = date_time;
        this->level = lv;
        this->threadName = std::move(thread_name);
        this->threadId = id;
        this->file = std::move(file);
        this->line = line;
        this->message = std::move(msg);
    }

    [[nodiscard]] const DateTime &getTime() const noexcept { return this->dateTime; }
    [[nodiscard]] Level getLevel() const noexcept { return this->level; }
    [[nodiscard]] tid_t getThreadId() const noexcept { return this->threadId; }
    [[nodiscard]] int32_t getLine() const noexcept { return this->line; }
    [[nodiscard]] auto &getFileName() const noexcept { return this->file; }
    [[nodiscard]] auto &getMessage() const noexcept { return this->message; }
    [[nodiscard]] auto &getThreadName() const noexcept { return this->threadName; }

private:
    DateTime dateTime;
    Level level;
    std::string threadName;
    tid_t threadId;
    std::string file;
    int32_t line;
    std::string message;
};
} // namespace sese::record