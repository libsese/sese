#pragma once

#include <sese/util/ByteBuilder.h>

namespace sese::net::ws {

    struct WebsocketSession {
        ByteBuilder recvBuffer{4096};
        ByteBuilder sendBuffer{4096};
    };
}