#pragma once
#include "AbstractAppender.h"

namespace sese {
    class API ConsoleAppender : public AbstractAppender {
    public:
        void dump(const Event::Ptr &event) noexcept override;
    };
}// namespace sese