#include <sese/res/ResourceStream.h>

sese::res::ResourceStream::ResourceStream(const void *buf, size_t size) : buf(buf), size(size) {}

int64_t sese::res::ResourceStream::read(void *buffer, size_t length) {
    auto readable = size - pos - 1;
    if (readable > 0) {
        std::memcpy(buffer, static_cast<const char *>(buf) + pos, readable);
        pos += readable;
    }
    return static_cast<int64_t>(readable);
}

int64_t sese::res::ResourceStream::peek(void *buffer, size_t length) {
    auto readable = size - pos - 1;
    if (readable > 0) {
        std::memcpy(buffer, static_cast<const char *>(buf) + pos, readable);
    }
    return static_cast<int64_t>(readable);
}

int64_t sese::res::ResourceStream::trunc(size_t length) {
    auto readable = size - pos - 1;
    if (readable > 0) {
        pos += readable;
    }
    return static_cast<int64_t>(readable);
}

int64_t sese::res::ResourceStream::getSeek() const {
    return static_cast<int64_t>(pos);
}

int32_t sese::res::ResourceStream::setSeek(int64_t offset, io::Seek seek) {
    return setSeek(offset, static_cast<int32_t>(seek));
}

int32_t sese::res::ResourceStream::setSeek(int64_t offset, int32_t whence) {
    int64_t new_pos;
    if (whence == SEEK_CUR) {
        new_pos = static_cast<int64_t>(pos) + offset;
    } else if (whence == SEEK_SET) {
        new_pos = offset;
    } else if (whence == SEEK_END) {
        new_pos = static_cast<int64_t>(size) - 1 + offset;
    } else {
        return -1;
    }
    if (new_pos < 0 || new_pos - 1 > static_cast<int64_t>(size)) {
        return -1;
    }
    pos = static_cast<size_t>(new_pos);
    return 0;
}
