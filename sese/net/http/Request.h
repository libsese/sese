/**
* @file Request.h
* @author kaoru
* @version 0.1
* @brief HTTP 请求类
* @date 2023年9月13日
*/

#pragma once

#include <sese/net/http/RequestHeader.h>
#include "sese/io/ByteBuilder.h"

namespace sese::net::http {

/// HTTP 请求类
class Request final : public RequestHeader {
public:
    using Ptr = std::unique_ptr<Request>;

    io::ByteBuilder &getBody() { return body; }

    void swap(Request &another) noexcept;

private:
    io::ByteBuilder body{8192};
};

} // namespace sese::net::http