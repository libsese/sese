#pragma once

#include <sese/service/Service.h>
#include <sese/net/http/ControllerGroup.h>
#include <sese/net/IPv6Address.h>
#include <sese/security/SSLContext.h>
#include <sese/thread/Thread.h>

namespace sese::service::http::v3 {

class HttpService : public Service {
public:
    using Ptr = std::shared_ptr<HttpService>;

    static HttpService::Ptr create();

    void setAddress(const net::IPAddress::Ptr &address) {
        this->address = address;
    }
    void setSSLContext(const security::SSLContext::Ptr &ssl_context) {
        this->ssl_context = ssl_context;
    }
    void setKeepalive(uint32_t seconds) {
        this->keepalive = seconds;
    }
    void setName(const std::string &name) {
        this->serv_name = name;
    }
    void regController(const std::string &http_url, const net::http::Controller &controller) {
        this->controllers[http_url] = controller;
    }
    void regMount(const std::string &local_src, const std::string &http_url_prefix) {
        this->mount_points[http_url_prefix] = local_src;
    }

protected:
    net::IPAddress::Ptr address;
    security::SSLContext::Ptr ssl_context;
    Thread::Ptr thread;
    uint32_t keepalive = 30;
    std::string serv_name;
    std::map<std::string, std::string> mount_points;
    std::map<std::string, net::http::Controller> controllers;
};

} // namespace sese::service::http::v3