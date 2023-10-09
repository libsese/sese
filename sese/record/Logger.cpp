#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "sese/record/Logger.h"
#include "sese/record/ConsoleAppender.h"
#include "sese/record/SimpleFormatter.h"

#include <clocale>

namespace sese::record {

Logger::Logger() noexcept {
    formatter = std::make_shared<SimpleFormatter>();
    builtInAppender = std::make_shared<ConsoleAppender>();
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
    // 内建控制台输出地不对日志等级做任何限制
    // if (builtInAppender->getLevel() <= event->getLevel()) {
    setbuf(stdout, nullptr);
    // 此处逻辑本身就是完备的
    switch (event->getLevel()) { // GCOVR_EXCL_LINE
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
    // }

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
    builtInAppender->dump((const char *) buffer, length);
    for (auto &appender: appenderVector) {
        appender->dump((const char *) buffer, length);
    }
}

Logger *getLogger() noexcept { return logger; }

int32_t LoggerInitiateTask::init() noexcept {
    // 初始化 Logger
    setlocale(LC_ALL, "");
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