#include <sese/net/http/UrlHelper.h>

using sese::http::UrlHelper;
using sese::http::UrlInfo;

void UrlHelper::parse(const std::string &url, UrlInfo &info) noexcept {
    // 协议
    auto protocolEnd = url.find("://", 0);
    if (protocolEnd != std::string::npos) {
        info.protocol = std::string_view(url.data(), protocolEnd);
        protocolEnd += 3;
    }

    // 域名
    auto hostEnd = url.find('/', protocolEnd);
    if (hostEnd != std::string::npos) {
        info.host = std::string_view(url.data() + protocolEnd, hostEnd - protocolEnd);
    }

    // 资源 & 查询字符串
    auto uriEnd = url.find('?', hostEnd);
    if (uriEnd == std::string::npos) {
        // 无查询字符串
        info.uri = std::string_view(url.data() + hostEnd);
    } else {
        // 有查询字符串
        info.uri = std::string_view(url.data() + hostEnd, uriEnd - hostEnd);
        info.query = std::string_view(url.data() + uriEnd);
    }
}