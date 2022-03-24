#pragma once
#include "AbstractAppender.h"

namespace sese {
    class API ConsoleAppender : public AbstractAppender {
    public:
        explicit ConsoleAppender(const Formatter::Ptr &formatter, Level level = Level::DEBUG) noexcept;

        void dump(const Event::Ptr &event) noexcept override;

    private:
    };
}// namespace sese