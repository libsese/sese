#pragma once
#include "AbstractAppender.h"
#include <vector>
#include <memory>
#include "Config.h"

#ifdef _WIN32
#pragma warning (disable : 4251)
#endif

namespace sese {
    class API Logger{
    public:
        typedef std::shared_ptr<Logger> Ptr;

        explicit Logger() noexcept = default;

        void addAppender(const AbstractAppender::Ptr &appender) noexcept;

        void log(const Event::Ptr &event) const noexcept;

    private:
        std::vector<AbstractAppender::Ptr> appenders;
    };
}