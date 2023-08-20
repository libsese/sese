#pragma once

#include <sese/net/http/ResponseHeader.h>
#include <sese/util/ByteBuilder.h>

namespace sese::net::http {

    class Response final : public ResponseHeader {
    public:
        ByteBuilder &getBody() { return body; }

        void swap(Response &another) noexcept;

    private:
        ByteBuilder body{8192};
    };

}// namespace sese::net::http