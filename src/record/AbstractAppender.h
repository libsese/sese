#pragma once
#include "Formatter.h"
#include "Config.h"

#ifdef _WIN32
#pragma warning (disable : 4251)
#endif

namespace sese {
    class API AbstractAppender {
    public:
        typedef std::shared_ptr<AbstractAppender> Ptr;

        AbstractAppender() noexcept = default;

        AbstractAppender(Formatter::Ptr formatter, Level level) noexcept;

        void preDump(const Event::Ptr &event) noexcept;

        virtual void dump(const Event::Ptr &event) noexcept = 0;

    protected:
        Formatter::Ptr formatter;
        Level level = Level::DEBUG;
    };
}// namespace sese