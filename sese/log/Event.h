// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file Event.h
 * @brief Log event class
 * @author kaoru
 * @date March 28, 2022
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
namespace sese::log {

/// \brief Log level
enum class Level {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERR = 3
};

/**
 * @brief Log event class
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
} // namespace sese::log