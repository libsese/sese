/**
 * @file UrlHelper.h
 * @date 2022年6月6日
 * @author kaoru
 * @brief Url 解析器
 */
#pragma once
#include <sese/Config.h>

namespace sese::net::http {

    /// Url 信息
    struct UrlInfo {
        /// 协议
        std::string_view protocol;
        /// 域名
        std::string_view host;
        /// 资源
        std::string_view uri;
        /// 查询
        std::string_view query;
    };

    /// Url 解析器
    class API UrlHelper final {
    public:
        static void parse(const std::string &url, UrlInfo &info) noexcept;
    };
}