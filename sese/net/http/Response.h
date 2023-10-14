/**
* @file Response.h
* @author kaoru
* @version 0.1
* @brief HTTP 响应类
* @date 2023年9月13日
*/

#pragma once

#include <sese/net/http/ResponseHeader.h>
#include "sese/io/ByteBuilder.h"

namespace sese::net::http {

/// HTTP 响应类
class Response final : public ResponseHeader {
public:
    using Ptr = std::unique_ptr<Response>;

    io::ByteBuilder &getBody() { return body; }

    void swap(Response &another) noexcept;

private:
    io::ByteBuilder body{8192};
};

} // namespace sese::net::http