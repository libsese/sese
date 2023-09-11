#pragma once

#include <sese/net/http/ResponseHeader.h>
#include "sese/io/ByteBuilder.h"

namespace sese::net::http {

    class Response final : public ResponseHeader {
    public:
        io::ByteBuilder &getBody() { return body; }

        void swap(Response &another) noexcept;

    private:
        io::ByteBuilder body{8192};
    };

}// namespace sese::net::http