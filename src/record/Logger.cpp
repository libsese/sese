#include "sese/record/Logger.h"
#include "sese/Singleton.h"
#include "sese/record/ConsoleAppender.h"
#include "sese/record/SimpleFormatter.h"
#include <clocale>

namespace sese::record {

    void Logger::addAppender(const AbstractAppender::Ptr &appender) noexcept {
        this->appenders.emplace_back(appender);
    }

    void Logger::log(const Event::Ptr &event) const noexcept {
        for (auto &appender: appenders) {
            appender->preDump(event);
        }
    }

    static Logger *logger = nullptr;

    Logger *getLogger() noexcept { return logger; }

    int32_t LoggerInitiateTask::init() noexcept {
        // 初始化 Logger
        setlocale(LC_ALL, "");
        logger = new Logger();
        auto formatter = std::make_shared<SimpleFormatter>();
        auto appender = std::make_shared<ConsoleAppender>(formatter);
        logger->addAppender(appender);
        return 0;
    }

    int32_t LoggerInitiateTask::destroy() noexcept {
        delete logger;
        return 0;
    }
}// namespace sese::record