#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "sese/record/AsyncLogger.h"
#include "sese/record/ConsoleAppender.h"
#include "sese/record/SimpleFormatter.h"
#include "sese/thread/Thread.h"

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
        for(auto iterator = appenderVector.begin(); iterator < appenderVector.end(); iterator++) {
            if (*iterator == appender) {
                this->appenderVector.erase(iterator);
                return;
            }
        }
    }

    void Logger::log(const Event::Ptr &event) noexcept {
        std::string content = formatter->dump(event);
        if (builtInAppender->getLevel() <= event->getLevel()) {
#ifdef USE_ASYNC_LOGGER
            setbuf(stdout, nullptr);
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
#endif
            builtInAppender->dump(content.c_str(), content.length());
            builtInAppender->dump("\n", 1);
#ifdef USE_ASYNC_LOGGER
            ConsoleAppender::setCleanColor();
            fflush(stdout);
#endif
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
        delete logger;
        return 0;
    }
}// namespace sese::record

#ifdef _WIN32
#undef _CRT_SECURE_NO_WARNINGS
#endif