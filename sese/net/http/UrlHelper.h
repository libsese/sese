/**
 * @file UrlHelper.h
 * @date 2022年6月6日
 * @author kaoru
 * @brief Url 解析器
 */

#pragma once

#if defined(_MSC_VER)
#pragma warning(disable : 4251)
#endif

#include <sese/Config.h>

namespace sese::net::http {

/// Url 解析器
class API Url final {
public:
    Url() = default;

    explicit Url(const std::string &url) noexcept;

    [[nodiscard]] const std::string &getProtocol() const {
        return protocol;
    }

    [[nodiscard]] const std::string &getHost() const {
        return host;
    }

    [[nodiscard]] const std::string &getUrl() const {
        return url;
    }

    [[nodiscard]] const std::string &getQuery() const {
        return query;
    }

private:
    /// 协议
    std::string protocol;
    /// 域名
    std::string host;
    /// 资源
    std::string url;
    /// 查询
    std::string query;
};
} // namespace sese::net::http