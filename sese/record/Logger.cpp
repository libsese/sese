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

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "sese/record/Logger.h"
#include "sese/record/ConsoleAppender.h"
#include "sese/record/SimpleFormatter.h"

#ifdef USE_ASYNC_LOGGER
#include "sese/record/AsyncLogger.h"
#endif

#include <clocale>

namespace sese::record {

Logger::Logger() noexcept {
    formatter = std::make_shared<SimpleFormatter>();
    builtInAppender = std::make_shared<ConsoleAppender>();
#ifdef SESE_IS_DEBUG
    builtInAppender->setLevel(sese::record::Level::DEBUG);
#else
    builtInAppender->setLevel(sese::record::Level::INFO);
#endif
}

void Logger::addAppender(const AbstractAppender::Ptr &appender) noexcept {
    this->appenderVector.emplace_back(appender);
}

void Logger::removeAppender(const AbstractAppender::Ptr &appender) noexcept {
    // 此处删除逻辑无需验证
    // GCOVR_EXCL_START
    for (auto iterator = appenderVector.begin(); iterator < appenderVector.end(); iterator++) {
        if (*iterator == appender) {
            this->appenderVector.erase(iterator);
            return;
        }
    }
    // GCOVR_EXCL_STOP
}

void Logger::log(const Event::Ptr &event) noexcept {
    std::string content = formatter->dump(event);
    if (builtInAppender->getLevel() <= event->getLevel()) {
        setbuf(stdout, nullptr);
        // 此处逻辑本身就是完备的
        switch (event->getLevel()) {
            case Level::DEBUG:
                ConsoleAppender::setDebugColor();
                break;
            case Level::INFO:
                ConsoleAppender::setInfoColor();
                break;
            case Level::WARN:
                ConsoleAppender::setWarnColor();
                break;
            case Level::ERR:
                ConsoleAppender::setErrorColor();
                break;
        }
        builtInAppender->dump(content.c_str(), content.length());
        ConsoleAppender::setCleanColor();
        fflush(stdout);
    }

    for (auto &appender: appenderVector) {
        if (appender->getLevel() <= event->getLevel()) {
            appender->dump(content.c_str(), content.length());
        }
    }
}

static Logger *logger = nullptr;

void Logger::addGlobalLoggerAppender(const AbstractAppender::Ptr &appender) noexcept {
    logger->addAppender(appender); // NOLINT
}

void Logger::removeGlobalLoggerAppender(const AbstractAppender::Ptr &appender) noexcept {
    logger->removeAppender(appender); // NOLINT
}

void Logger::dump(const void *buffer, size_t length) noexcept {
    builtInAppender->dump(static_cast<const char *>(buffer), length);
    for (auto &appender: appenderVector) {
        appender->dump(static_cast<const char *>(buffer), length);
    }
}

Logger *getLogger() noexcept { return logger; }

int32_t LoggerInitiateTask::init() noexcept {
    // 初始化 Logger
#ifdef USE_ASYNC_LOGGER
    logger = new AsyncLogger();
#else
    logger = new Logger();
#endif
    return 0;
}

int32_t LoggerInitiateTask::destroy() noexcept {
    delete logger; // GCOVR_EXCL_LINE
    return 0;
}
} // namespace sese::record

#ifdef _WIN32
#undef _CRT_SECURE_NO_WARNINGS
#endif