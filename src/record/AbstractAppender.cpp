#include "sese/record/AbstractAppender.h"

namespace sese {
    record::AbstractAppender::AbstractAppender(AbstractFormatter::Ptr formatter, Level level) noexcept {
        this->formatter = std::move(formatter);
        this->level = level;
    }

    void record::AbstractAppender::preDump(const Event::Ptr &event) noexcept {
        if (event->getLevel() >= level) {
            dump(event);
        }
    }
}// namespace sese