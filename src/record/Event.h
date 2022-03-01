#pragma once
#include <memory>
#include "Config.h"

namespace sese {

    enum class Level {
        DEBUG = 0,
        INFO = 1,
        WARN = 2,
        ERROR = 3
    };

    class API Event {
    public:
        typedef std::shared_ptr<Event> Ptr;

    public:
        Event(time_t tm, Level lv, const char *threadName, pid_t id, const char *file, int line, const char *msg) noexcept {
            this->time = tm;
            this->level = lv;
            this->threadName = threadName;
            this->threadId = id;
            this->file = file;
            this->line = line;
            this->message = msg;
        }

        [[nodiscard]] time_t getTime() const noexcept { return this->time; }
        [[nodiscard]] Level getLevel() const noexcept { return this->level; }
        [[nodiscard]] pid_t getThreadId() const noexcept { return this->threadId; }
        [[nodiscard]] int getLine() const noexcept { return this->line; }
        [[nodiscard]] const char *getFileName() const noexcept { return this->file; }
        [[nodiscard]] const char *getMessage() const noexcept { return this->message; }
        [[nodiscard]] const char *getThreadName() const noexcept { return this->threadName; }

    private:
        time_t time;
        Level level;
        const char *threadName = nullptr;
        pid_t threadId;
        const char *file = nullptr;
        int line;
        const char *message = nullptr;
    };
}// namespace sese