#include "sese/record/FileAppender.h"

void sese::FileAppender::dump(const sese::Event::Ptr &event) noexcept {
    auto str = this->formatter->dump(event);
    fileStream->write((void *)str.c_str(), str.length());
    fileStream->write((void *)"\n", 1);
}
