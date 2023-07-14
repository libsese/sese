#include "sese/util/FixedBuffer.h"
#include "sese/thread/Locker.h"

// GCOVR_EXCL_START

sese::FixedBuffer::FixedBuffer(size_t size) noexcept : AbstractFixedBuffer(size) {}

int64_t sese::FixedBuffer::read(void *buffer, size_t length) {
    Locker locker(mutex);
    return AbstractFixedBuffer::read(buffer, length);
}

int64_t sese::FixedBuffer::write(const void *buffer, size_t length) {
    Locker locker(mutex);
    return AbstractFixedBuffer::write(buffer, length);
}

int64_t sese::FixedBuffer::peek(void *buffer, size_t length) {
    Locker locker(mutex);
    return AbstractFixedBuffer::peek(buffer, length);
}

int64_t sese::FixedBuffer::trunc(size_t length) {
    Locker locker(mutex);
    return AbstractFixedBuffer::trunc(length);
}

void sese::FixedBuffer::reset() noexcept {
    Locker locker(mutex);
    AbstractFixedBuffer::reset();
}

size_t sese::FixedBuffer::getReadableSize() noexcept {
    Locker locker(mutex);
    return AbstractFixedBuffer::getReadableSize();
}

size_t sese::FixedBuffer::getWriteableSize() noexcept {
    Locker locker(mutex);
    return AbstractFixedBuffer::getWriteableSize();
}

// GCOVR_EXCL_STOP