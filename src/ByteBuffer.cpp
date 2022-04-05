#include "ByteBuffer.h"
#include "thread/Locker.h"

int64_t sese::ByteBuffer::read(void *buffer, size_t len) {
    Locker locker(mutex);
    return AbstractByteBuffer::read(buffer, len);
}

int64_t sese::ByteBuffer::write(void *buffer, size_t needWrite) {
    Locker locker(mutex);
    return AbstractByteBuffer::write(buffer, needWrite);
}

void sese::ByteBuffer::close() {
    Locker locker(mutex);
    AbstractByteBuffer::close();
}

sese::ByteBuffer::ByteBuffer(size_t baseSize) : AbstractByteBuffer(baseSize) {
}

void sese::ByteBuffer::resetPos() {
    Locker locker(mutex);
    AbstractByteBuffer::resetPos();
}

size_t sese::ByteBuffer::getLength() {
    Locker locker(mutex);
    return AbstractByteBuffer::getLength();
}

size_t sese::ByteBuffer::getCapacity() {
    Locker locker(mutex);
    return AbstractByteBuffer::getCapacity();
}

size_t sese::ByteBuffer::freeCapacity() {
    Locker locker(mutex);
    return AbstractByteBuffer::freeCapacity();
}
