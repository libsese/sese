#include "sese/record/FileAppender.h"

sese::record::FileAppender::FileAppender(FileStream::Ptr fileStream, sese::record::Level level)
    : AbstractAppender(level),
      fileStream(std::move(fileStream)) {
    bufferedStream = std::make_shared<BufferedStream>(this->fileStream);
}

sese::record::FileAppender::~FileAppender() noexcept {
    bufferedStream->flush();
    fileStream->close();
}

void sese::record::FileAppender::dump(const char *buffer, size_t size) noexcept {
    bufferedStream->write((void *) buffer, size);
    bufferedStream->write((void *) "\n", 1);
}
