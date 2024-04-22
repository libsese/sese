#pragma once

#include <sese/service/Service.h>
#include <sese/service/http/HttpConnection_V3.h>
#include <sese/net/http/ControllerGroup.h>
#include <sese/net/IPv6Address.h>
#include <sese/security/SSLContext.h>

namespace sese::service::http::v3 {

class HttpService final : public Service {
public:
    bool startup() override;
    bool shutdown() override;
    int getLastError() override;

    HttpService &&setAddress(const net::IPAddress::Ptr &address) && {
        this->address = address;
        return std::move(*this);
    }
    HttpService &&setSSLContext(const security::SSLContext::Ptr &ssl_context) && {
        this->ssl_context = ssl_context;
        return std::move(*this);
    }
    HttpService &&setKeepalive(uint32_t seconds) && {
        this->keepalive = seconds;
        return std::move(*this);
    }
    HttpService &&setName(const std::string &name) && {
        this->serv_name = name;
        return std::move(*this);
    }
    HttpService &&regController(const std::string &http_url, const net::http::Controller &controller) && {
        this->controllers[http_url] = controller;
        return std::move(*this);
    }
    HttpService &&regMount(const std::string &local_src, const std::string &http_url_prefix) && {
        this->mount_points[http_url_prefix] = local_src;
        return std::move(*this);
    }

private:
    net::IPAddress::Ptr address;
    security::SSLContext::Ptr ssl_context;
    uint32_t keepalive = 30;
    std::string serv_name;
    std::map<std::string, std::string> mount_points;
    std::map<std::string, net::http::Controller> controllers;
};

} // namespace sese::service::http::v3