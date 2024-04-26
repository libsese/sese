#pragma once

#include <sese/service/Service.h>
#include <sese/net/http/Controller.h>
#include <sese/net/IPv6Address.h>
#include <sese/security/SSLContext.h>
#include <sese/thread/Thread.h>

namespace sese::service::http::v3 {

class HttpService : public Service {
public:
    using Ptr = std::shared_ptr<HttpService>;

    static HttpService::Ptr create();

    HttpService &setAddress(const net::IPAddress::Ptr &address) & {
        this->address = address;
        return *this;
    }
    HttpService &setSSLContext(const security::SSLContext::Ptr &ssl_context) & {
        this->ssl_context = ssl_context;
        return *this;
    }
    HttpService &setKeepalive(uint32_t seconds) & {
        this->keepalive = std::max<uint32_t>(5, seconds);
        return *this;
    }
    HttpService &setName(const std::string &name) & {
        this->serv_name = name;
        return *this;
    }
    HttpService &regController(const net::http::Controller &controller) & {
        this->controllers[controller.getUri()] = controller;
        return *this;
    }
    HttpService &regMount(const std::string &local_src, const std::string &http_url_prefix) & {
        this->mount_points[http_url_prefix] = local_src;
        return *this;
    }

protected:
    net::IPAddress::Ptr address;
    security::SSLContext::Ptr ssl_context;
    Thread::Ptr thread;
    uint32_t keepalive = 30;
    std::string serv_name = "HttpSerivce";
    std::map<std::string, std::string> mount_points;
    std::map<std::string, net::http::Controller> controllers;
};

} // namespace sese::service::http::v3