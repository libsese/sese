/// \file RequestParser.h
/// \author kaoru
/// \brief 请求解析器
/// \date 2023年10月14日

#pragma once

#include <sese/net/IPv6Address.h>
#include <sese/net/http/Request.h>
#include <sese/net/http/UrlHelper.h>

namespace sese::net::http {

/// 请求解析器
class RequestParser {
public:
    /// 请求解析结果
    struct Result {
        Url url;
        IPAddress::Ptr address{};
        Request::Ptr request{};
    };

    static Result parse(const std::string &url);

    static IPv4Address::Ptr parseAddress(const std::string &host) noexcept;
};

} // namespace sese::net::http
