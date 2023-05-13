#include "sese/util/FixedBuffer.h"

sese::FixedBuffer::FixedBuffer(size_t size) noexcept
    : size(size),
      buffer(new char[size]) {
}

sese::FixedBuffer::~FixedBuffer() noexcept {
    if (buffer) {
        delete[] buffer;
    }
}

sese::FixedBuffer::FixedBuffer(const sese::FixedBuffer &buffer) noexcept {
    size = buffer.size;
    readSize = buffer.readSize;
    writeSize = buffer.writeSize;
    this->buffer = new char[size];
    memcpy(this->buffer, buffer.buffer, size);
}

sese::FixedBuffer::FixedBuffer(sese::FixedBuffer &&buffer) noexcept {
    this->size = buffer.size;
    this->readSize = buffer.readSize;
    this->writeSize = buffer.writeSize;
    this->buffer = buffer.buffer;

    buffer.size = 0;
    buffer.readSize = 0;
    buffer.writeSize = 0;
    buffer.buffer = nullptr;
}

int64_t sese::FixedBuffer::read(void *buf, size_t length) {
    if (writeSize - readSize >= length ) {
        memcpy(buf, buffer + readSize, length);
        readSize += length;
        return (int64_t) length;
    } else {
        auto read = writeSize - readSize;
        memcpy(buf, buffer + readSize, read);
        readSize += read;
        return (int64_t) read;
    }
}

int64_t sese::FixedBuffer::write(const void *buf, size_t length) {
    if (size - writeSize >= length) {
        memcpy(buffer + writeSize, buf, length);
        writeSize += length;
        return (int64_t) length;
    } else {
        auto write = size - writeSize;
        memcpy(buffer + writeSize, buf, write);
        writeSize += write;
        return (int64_t) write;
    }
}

int64_t sese::FixedBuffer::peek(void *buf, size_t length) {
    if (writeSize - readSize >= length ) {
        memcpy(buf, buffer + readSize, length);
        // readSize += length;
        return (int64_t) length;
    } else {
        auto read = writeSize - readSize;
        memcpy(buf, buffer + readSize, read);
        // readSize += read;
        return (int64_t) read;
    }
}

void sese::FixedBuffer::reset() noexcept {
    readSize = writeSize = 0;
}
