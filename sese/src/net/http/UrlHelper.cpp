#include <sese/net/http/UrlHelper.h>

using sese::net::http::Url;

Url::Url(const std::string &url) noexcept {
    // 协议
    auto protocolEnd = url.find("://", 0);
    if (protocolEnd != std::string::npos) {
        protocol = std::string_view(url.data(), protocolEnd);
        protocolEnd += 3;
    } else {
        protocolEnd = 0;
    }

    // 域名
    auto hostEnd = url.find('/', protocolEnd);
    if (hostEnd != std::string::npos) {
        host = std::string_view(url.data() + protocolEnd, hostEnd - protocolEnd);
    } else {
        hostEnd = protocolEnd;
    }

    // 资源 & 查询字符串
    auto uriEnd = url.find('?', hostEnd);
    if (uriEnd == std::string::npos) {
        // 无查询字符串
        this->url = std::string_view(url.data() + hostEnd);
    } else {
        // 有查询字符串
        this->url = std::string_view(url.data() + hostEnd, uriEnd - hostEnd);
        query = std::string_view(url.data() + uriEnd);
    }
}