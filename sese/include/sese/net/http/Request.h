#pragma once

#include <sese/net/http/RequestHeader.h>
#include "sese/io/ByteBuilder.h"

namespace sese::net::http {

    class Request final : public RequestHeader {
    public:
        io::ByteBuilder &getBody() { return body; }

        void swap(Request &another) noexcept;

    private:
        io::ByteBuilder body{8192};
    };

}// namespace sese::net::http