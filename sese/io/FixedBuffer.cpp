#include "sese/io/FixedBuffer.h"
#include "sese/thread/Locker.h"

// GCOVR_EXCL_START

sese::io::FixedBuffer::FixedBuffer(size_t size) noexcept : AbstractFixedBuffer(size) {}

int64_t sese::io::FixedBuffer::read(void *buffer, size_t length) {
    Locker locker(mutex);
    return AbstractFixedBuffer::read(buffer, length);
}

int64_t sese::io::FixedBuffer::write(const void *buffer, size_t length) {
    Locker locker(mutex);
    return AbstractFixedBuffer::write(buffer, length);
}

int64_t sese::io::FixedBuffer::peek(void *buffer, size_t length) {
    Locker locker(mutex);
    return AbstractFixedBuffer::peek(buffer, length);
}

int64_t sese::io::FixedBuffer::trunc(size_t length) {
    Locker locker(mutex);
    return AbstractFixedBuffer::trunc(length);
}

void sese::io::FixedBuffer::reset() noexcept {
    Locker locker(mutex);
    AbstractFixedBuffer::reset();
}

size_t sese::io::FixedBuffer::getReadableSize() noexcept {
    Locker locker(mutex);
    return AbstractFixedBuffer::getReadableSize();
}

size_t sese::io::FixedBuffer::getWriteableSize() noexcept {
    Locker locker(mutex);
    return AbstractFixedBuffer::getWriteableSize();
}

// GCOVR_EXCL_STOP