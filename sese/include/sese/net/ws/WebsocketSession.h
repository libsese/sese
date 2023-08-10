#pragma once

#include <sese/util/ByteBuilder.h>

namespace sese::net::ws {
    struct WebsocketSession {
        int fd = 0;
        ByteBuilder buffer{4096};
        void *data = nullptr;
    };
}// namespace sese::net::ws