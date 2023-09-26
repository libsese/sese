#include "sese/record/BlockAppender.h"
#include "sese/text/DateTimeFormatter.h"
#include "sese/thread/Locker.h"

#include <algorithm>

using namespace sese;
using namespace sese::record;

inline std::string getDateTimeString() {
    auto dateTime = DateTime::now();
    return text::DateTimeFormatter::format(dateTime, RECORD_DEFAULT_FILE_TIME_PATTERN) + ".log";
}

BlockAppender::BlockAppender(size_t blockMaxSize, sese::record::Level level)
    : AbstractAppender(level), maxSize(blockMaxSize) {
    // #ifndef _DEBUG
    //     maxSize = blockMaxSize < 1000 * 1024 ? 1000 * 1024 : blockMaxSize;
    // #endif
    maxSize = std::max<decltype(maxSize)>(1024 * 1000, maxSize);
    auto fileName = getDateTimeString();
    fileStream = io::FileStream::create(fileName, TEXT_WRITE_CREATE_TRUNC);
    bufferedStream = std::make_unique<io::BufferedStream>(fileStream, 4 * 1024);
}

BlockAppender::~BlockAppender() noexcept {
    bufferedStream->flush();
    fileStream->close();
}

void BlockAppender::dump(const char *buffer, size_t i) noexcept {
    Locker locker(this->mutex);
    if (this->size + i > this->maxSize) {
        this->size = i;
        bufferedStream->flush();
        fileStream->close();
        auto fileName = getDateTimeString();
        fileStream = io::FileStream::create(fileName, TEXT_WRITE_CREATE_TRUNC);
        bufferedStream->reset(fileStream);
        bufferedStream->write((void *) buffer, i);
    } else {
        this->size += i;
        bufferedStream->write((void *) buffer, i);
    }
}
