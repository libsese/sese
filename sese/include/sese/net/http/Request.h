#pragma once

#include <sese/net/http/RequestHeader.h>
#include <sese/util/ByteBuilder.h>

namespace sese::net::http {

    class Request final : public RequestHeader {
    public:
        ByteBuilder &getBody() { return body; }

    private:
        ByteBuilder body;
    };

}// namespace sese::net::http