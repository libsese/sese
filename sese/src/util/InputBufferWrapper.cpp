#include "sese/util/InputBufferWrapper.h"
#include "sese/util/OutputStream.h"

sese::InputBufferWrapper::InputBufferWrapper(const char *buffer, size_t cap) {
    this->buffer = buffer;
    this->cap = cap;
}

int64_t sese::InputBufferWrapper::read(void *buf, size_t length) {
    if (pos == cap) {
        return 0;
    } else if ((cap - pos) >= length) {
        memcpy(buf, buffer + pos, length);
        pos += length;
        return (int64_t) length;
    } else {
        auto remain = cap - pos;
        memcpy(buf, buffer + pos, remain);
        pos = cap;
        return (int64_t) (remain);
    }
}

int64_t sese::InputBufferWrapper::peek(void *buf, size_t length) {
    if (pos == cap) {
        return 0;
    } else if ((cap - pos) >= length) {
        memcpy(buf, buffer + pos, length);
        return (int64_t) length;
    } else {
        auto remain = cap - pos;
        memcpy(buf, buffer + pos, remain);
        return (int64_t) (remain);
    }
}

const char *sese::InputBufferWrapper::getBuffer() const {
    return buffer;
}

size_t sese::InputBufferWrapper::getLen() const {
    return pos;
}

size_t sese::InputBufferWrapper::getCap() const {
    return cap;
}

void sese::InputBufferWrapper::reset() noexcept {
    this->pos = 0;
}

int64_t sese::InputBufferWrapper::trunc(size_t length) {
    if (pos == cap) {
        return 0;
    } else if ((cap - pos) >= length) {
        // memcpy(buf, buffer + pos, length);
        pos += length;
        return (int64_t) length;
    } else {
        auto remain = cap - pos;
        // memcpy(buf, buffer + pos, remain);
        pos = cap;
        return (int64_t) (remain);
    }
}

int64_t operator<<(sese::OutputStream &out, sese::InputBufferWrapper &input) noexcept {
    auto len = out.write(input.getBuffer() + input.getLen(), input.getCap() - input.getLen());
    input.trunc(len);
    return len;
}

int64_t operator<<(sese::OutputStream *out, sese::InputBufferWrapper &input) noexcept {
    auto len = out->write(input.getBuffer() + input.getLen(), input.getCap() - input.getLen());
    input.trunc(len);
    return len;
}