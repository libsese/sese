#include "Logger.h"

namespace sese {

    void Logger::addAppender(const sese::AbstractAppender::Ptr &appender) noexcept {
        this->appenders.emplace_back(appender);
    }

    void Logger::log(const Event::Ptr &event) const noexcept {
        event->setLogName(this->name);
        for (auto &appender: appenders) {
            appender->preDump(event);
        }
    }

}// namespace sese