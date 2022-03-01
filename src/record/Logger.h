#pragma once
#include "AbstractAppender.h"
#include "Config.h"
#include <memory>
#include <vector>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {
    class API Logger {
    public:
        typedef std::shared_ptr<Logger> Ptr;

        explicit Logger(const char *name = MASTER_LOGGER_NAME) noexcept {
            this->name = name;
        }

        void addAppender(const AbstractAppender::Ptr &appender) noexcept;

        void log(const Event::Ptr &event) const noexcept;

    private:
        const char *name = nullptr;
        std::vector<AbstractAppender::Ptr> appenders;
    };
}// namespace sese