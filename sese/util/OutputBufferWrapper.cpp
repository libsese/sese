#include "OutputBufferWrapper.h"

const char *sese::OutputBufferWrapper::getBuffer() const {
    return buffer;
}

size_t sese::OutputBufferWrapper::getLen() const {
    return len;
}

size_t sese::OutputBufferWrapper::getCap() const {
    return cap;
}

sese::OutputBufferWrapper::OutputBufferWrapper(char *buffer, size_t cap) {
    this->buffer = buffer;
    this->cap = cap;
}

int64_t sese::OutputBufferWrapper::write(const void *buf, size_t length) {
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

void sese::OutputBufferWrapper::reset() noexcept {
    this->len = 0;
}