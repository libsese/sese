#include "sese/util/AbstractFixedBuffer.h"

sese::AbstractFixedBuffer::AbstractFixedBuffer(size_t size) noexcept
    : size(size),
      buffer(new char[size]) {
}

sese::AbstractFixedBuffer::~AbstractFixedBuffer() noexcept {
    if (buffer) { // NOLINT
        delete[] buffer;
    }
}

sese::AbstractFixedBuffer::AbstractFixedBuffer(const sese::AbstractFixedBuffer &buffer) noexcept {
    size = buffer.size;
    readSize = buffer.readSize;
    writeSize = buffer.writeSize;
    this->buffer = new char[size];
    memcpy(this->buffer, buffer.buffer, size);
}

sese::AbstractFixedBuffer::AbstractFixedBuffer(sese::AbstractFixedBuffer &&buffer) noexcept {
    this->size = buffer.size;
    this->readSize = buffer.readSize;
    this->writeSize = buffer.writeSize;
    this->buffer = buffer.buffer;

    buffer.size = 0;
    buffer.readSize = 0;
    buffer.writeSize = 0;
    buffer.buffer = nullptr;
}

int64_t sese::AbstractFixedBuffer::read(void *buf, size_t length) {
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

int64_t sese::AbstractFixedBuffer::write(const void *buf, size_t length) {
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

int64_t sese::AbstractFixedBuffer::peek(void *buf, size_t length) {
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

void sese::AbstractFixedBuffer::reset() noexcept {
    readSize = writeSize = 0;
}

int64_t sese::AbstractFixedBuffer::trunc(size_t length) {
    if (writeSize - readSize >= length ) {
        // memcpy(buf, buffer + readSize, length);
        readSize += length;
        return (int64_t) length;
    } else {
        auto read = writeSize - readSize;
        // memcpy(buf, buffer + readSize, read);
        readSize += read;
        return (int64_t) read;
    }
}
