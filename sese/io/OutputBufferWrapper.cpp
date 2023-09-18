#include "sese/io/OutputBufferWrapper.h"

const char *sese::io::OutputBufferWrapper::getBuffer() const {
    return buffer;
}

size_t sese::io::OutputBufferWrapper::getLength() const {
    return len;
}

size_t sese::io::OutputBufferWrapper::getCapacity() const {
    return cap;
}

sese::io::OutputBufferWrapper::OutputBufferWrapper(char *buffer, size_t cap) {
    this->buffer = buffer;
    this->cap = cap;
}

int64_t sese::io::OutputBufferWrapper::write(const void *buf, size_t length) {
    auto remain = cap - len;
    if (remain == 0) {
        return 0;
    } else if (remain >= length) {
        memcpy(buffer + len, buf, length);
        len += length;
        return (int64_t) length;
    } else {
        memcpy(buffer + len, buf, remain);
        len = cap;
        return (int64_t) remain;
    }
}

void sese::io::OutputBufferWrapper::reset() noexcept {
    this->len = 0;
}