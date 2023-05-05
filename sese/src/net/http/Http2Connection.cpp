#include "sese/net/http/Http2Connection.h"

using namespace sese::net::http;

Http2Connection::Http2Connection(v2::IOContext ctx) noexcept : context(ctx) {}

void Http2Connection::addStream(uint32_t sid, const Http2Stream::Ptr &stream) noexcept {
    streamMap[sid] = stream;
}

Http2Stream::Ptr Http2Connection::find(uint32_t sid) {
    auto iterator = streamMap.find(sid);
    if (iterator != streamMap.end()) {
        return iterator->second;
    } else {
        return nullptr;
    }
}
