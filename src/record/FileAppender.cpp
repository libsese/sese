#include "sese/record/FileAppender.h"

void sese::record::FileAppender::dump(const char *buffer, size_t size) noexcept {
    fileStream->write((void *)buffer, size);
    fileStream->write((void *)"\n", 1);
}
