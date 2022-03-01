#include "AbstractAppender.h"

namespace sese {
    AbstractAppender::AbstractAppender(Formatter::Ptr formatter, Level level) noexcept {
        this->formatter = std::move(formatter);
        this->level = level;
    }

    void AbstractAppender::preDump(const Event::Ptr &event) noexcept {
        if (event->getLevel() >= level) {
            dump(event);
        }
    }
}// namespace sese