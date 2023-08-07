#pragma once

#include <sese/net/http/ResponseHeader.h>
#include <sese/util/ByteBuilder.h>

namespace sese::net::http {

    class Response final : public ResponseHeader {
    public:
        ByteBuilder &getBody() { return body; }

    private:
        ByteBuilder body;
    };

}// namespace sese::net::http