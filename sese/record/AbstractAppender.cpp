#include "sese/record/AbstractAppender.h"

namespace sese::record {
AbstractAppender::AbstractAppender(Level level) noexcept {
    this->level = level;
}
} // namespace sese::record