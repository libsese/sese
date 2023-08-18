#include "sese/net/http/Http2Stream.h"

using namespace sese::net::v2::http;

int64_t Http2Stream::write(const void *buf, size_t len) {
    return buffer.write(buf, len);
}

int64_t Http2Stream::read(void *buf, size_t len) {
    return buffer.read(buf, len);
}

void Http2Stream::close() {
    willClose = true;
}
