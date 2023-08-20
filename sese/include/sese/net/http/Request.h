#pragma once

#include <sese/net/http/RequestHeader.h>
#include <sese/util/ByteBuilder.h>

namespace sese::net::http {

    class Request final : public RequestHeader {
    public:
        ByteBuilder &getBody() { return body; }

        void swap(Request &another) noexcept;

    private:
        ByteBuilder body{8192};
    };

}// namespace sese::net::http