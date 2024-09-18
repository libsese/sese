/// @file HttpService.h
/// @brief Http v3 服务接口
/// @author kaoru
/// @date 2024年05月1日

#pragma once

#include <sese/service/Service.h>
#include <sese/net/http/Controller.h>
#include <sese/net/IPv6Address.h>
#include <sese/security/SSLContext.h>
#include <sese/thread/Thread.h>

#include <unordered_map>

namespace sese::service::http {

/// Http 服务接口
class HttpService : public Service {
public:
    using Ptr = std::shared_ptr<HttpService>;
    using SSLContextPtr = std::unique_ptr<security::SSLContext>;
    using FilterCallback = std::function<bool(net::http::Request &, net::http::Response &)>;
    using FilterMap = std::unordered_map<std::string, FilterCallback>;
    using MountPointMap = std::unordered_map<std::string, std::string>;
    using ServletMap = std::unordered_map<std::string, net::http::Servlet>;

    static HttpService::Ptr create(
            const net::IPAddress::Ptr &address,
            SSLContextPtr ssl_context,
            uint32_t keepalive,
            std::string &serv_name,
            MountPointMap &mount_points,
            ServletMap &servlets,
            FilterMap &filters
    );

protected:
    HttpService(
            net::IPAddress::Ptr address,
            SSLContextPtr ssl_context,
            uint32_t keepalive,
            std::string &serv_name,
            MountPointMap &mount_points,
            ServletMap &servlets,
            FilterMap &filters
    );

    net::IPAddress::Ptr address;
    SSLContextPtr ssl_context;
    Thread::Ptr thread;
    uint32_t keepalive = 30;
    std::string &serv_name;
    MountPointMap &mount_points;
    ServletMap &servlets;
    FilterMap &filters;
};

} // namespace sese::service::http