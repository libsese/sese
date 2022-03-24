#include "record/FileAppender.h"

sese::FileAppender::~FileAppender() {
    if (this->file) {
        fclose(file);
    }
}

void sese::FileAppender::dump(const sese::Event::Ptr &event) noexcept {
    auto str = this->formatter->dump(event);
    fwrite(str.c_str(), str.length(), 1, this->file);
    fwrite("\n", 1, 1, this->file);
}
