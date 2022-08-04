#include "sese/record/FileAppender.h"

void sese::record::FileAppender::dump(const record::Event::Ptr &event) noexcept {
    auto str = this->formatter->dump(event);
    fileStream->write((void *)str.c_str(), str.length());
    fileStream->write((void *)"\n", 1);
}
