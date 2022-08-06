#include <sese/record/BlockAppender.h>
#include <sese/DateTimeFormatter.h>
#include <sese/thread/Locker.h>

using namespace sese;
using namespace sese::record;

inline std::string getDateTimeString() {
    auto dateTime = DateTime::now();
    return DateTimeFormatter::format(dateTime, RECORD_DEFAULT_TIME_PATTERN);
}

BlockAppender::BlockAppender(size_t blockMaxSize, sese::record::Level level)
    : AbstractAppender(level) {
    maxSize = blockMaxSize;
    auto fileName = getDateTimeString();
    fileStream = FileStream::create(fileName, TEXT_WRITE_CREATE_TRUNC);
    bufferedStream = std::make_unique<BufferedStream>(fileStream);
}

BlockAppender::~BlockAppender() noexcept {
    bufferedStream->flush();
    fileStream->close();
}

void BlockAppender::dump(const char *buffer, size_t i) noexcept {
    if (this->size + i > this->maxSize) {
        Locker locker(this->mutex);
        this->size = i;
        bufferedStream->flush();
        fileStream->close();
        auto fileName = getDateTimeString();
        fileStream = FileStream::create(fileName, TEXT_WRITE_CREATE_TRUNC);
        bufferedStream->write((void *) buffer, i);
        bufferedStream->write((void *) "\n", 1);
    } else {
        Locker locker(this->mutex);
        this->size += i;
        bufferedStream->write((void *) buffer, i);
        bufferedStream->write((void *) "\n", 1);
    }
}
