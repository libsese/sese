#include "Logger.h"
#include <cstdio>

namespace sese {

    void Logger::addAppender(const sese::AbstractAppender::Ptr &appender) noexcept {
        this->appenders.emplace_back(appender);
    }

    void Logger::log(const Event::Ptr &event) const noexcept {
        for (auto &appender: appenders) {
            printf("%s ", this->name);
            appender->preDump(event);
        }
    }

}// namespace sese