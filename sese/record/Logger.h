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
 * @file Logger.h
 * @author kaoru
 * @date March 28, 2022
 * @brief Logger class
 */

#pragma once

#include "sese/Config.h"
#include "sese/record/AbstractAppender.h"
#include "sese/record/AbstractFormatter.h"
#include "sese/record/Event.h"
#include "sese/util/Initializer.h"
#include "sese/thread/Thread.h"

#include <memory>
#include <vector>
#include <source_location>

#ifdef _WIN32
#pragma warning(disable : 4251)
#pragma warning(disable : 4544)
#endif

namespace sese::record {

/// Logger initialization task
class LoggerInitiateTask final : public InitiateTask {
public:
    LoggerInitiateTask() : InitiateTask(__FUNCTION__) {}

    int32_t init() noexcept override;
    int32_t destroy() noexcept override;
};

class ConsoleAppender;

/**
 * @brief Logger class
 */
class Logger {
public:
    typedef std::shared_ptr<Logger> Ptr;

    Logger() noexcept;

    virtual ~Logger() noexcept = default;

    /**
     * Add log appender
     * @param appender Log appender
     */
    void addAppender(const AbstractAppender::Ptr &appender) noexcept;

    /**
     * Remove log appender
     * @param appender Log appender
     */
    void removeAppender(const AbstractAppender::Ptr &appender) noexcept;

    /**
     * Output log
     * @param event Log event
     */
    virtual void log(const Event::Ptr &event) noexcept;

    /**
     * Output raw content
     * @param buffer Buffer
     * @param length Length of buffer
     */
    virtual void dump(const void *buffer, size_t length) noexcept;

    struct PatternAndLocation {
        std::string_view pattern;
        std::source_location location;

        PatternAndLocation(const char *pattern, const std::source_location &location = std::source_location::current())
            : pattern(pattern), location(location) {
        }
    };

    template<typename... ARGS>
    static void debug(PatternAndLocation pattern_and_location, ARGS &&...args);

    static void debug(PatternAndLocation pattern_and_location);

    template<typename... ARGS>
    static void info(PatternAndLocation pattern_and_location, ARGS &&...args);

    static void info(PatternAndLocation pattern_and_location);

    template<typename... ARGS>
    static void warn(PatternAndLocation pattern_and_location, ARGS &&...args);

    static void warn(PatternAndLocation pattern_and_location);

    template<typename... ARGS>
    static void error(PatternAndLocation pattern_and_location, ARGS &&...args);

    static void error(PatternAndLocation pattern_and_location);

    /// Add log appender to global logger
    /// \param appender Log appender
    static void addGlobalLoggerAppender(const AbstractAppender::Ptr &appender) noexcept;

    /// Remove log appender from global logger
    /// \param appender Log appender
    static void removeGlobalLoggerAppender(const AbstractAppender::Ptr &appender) noexcept;

protected:
    std::shared_ptr<AbstractFormatter> formatter;
    std::shared_ptr<ConsoleAppender> builtInAppender;
    std::vector<AbstractAppender::Ptr> appenderVector;

private:
    static void prelog(PatternAndLocation &pattern_and_location, Level level, const std::string &string) noexcept;
};

/**
 * Get global Logger pointer
 * @return Logger pointer
 */
extern Logger *getLogger() noexcept;

template<typename... ARGS>
void Logger::debug(PatternAndLocation pattern_and_location, ARGS &&...args) {
    auto message = text::fmt(pattern_and_location.pattern, std::forward<ARGS>(args)...);
    prelog(pattern_and_location, Level::DEBUG, message);
}

template<typename... ARGS>
void Logger::info(PatternAndLocation pattern_and_location, ARGS &&...args) {
    auto message = text::fmt(pattern_and_location.pattern, std::forward<ARGS>(args)...);
    prelog(pattern_and_location, Level::INFO, message);
}

template<typename... ARGS>
void Logger::warn(PatternAndLocation pattern_and_location, ARGS &&...args) {
    auto message = text::fmt(pattern_and_location.pattern, std::forward<ARGS>(args)...);
    prelog(pattern_and_location, Level::WARN, message);
}

template<typename... ARGS>
void Logger::error(PatternAndLocation pattern_and_location, ARGS &&...args) {
    auto message = text::fmt(pattern_and_location.pattern, std::forward<ARGS>(args)...);
    prelog(pattern_and_location, Level::ERR, message);
}

} // namespace sese::record
