#pragma once

#include <sese/service/http/HttpService_V3.h>
#include <sese/net/http/Controller.h>

namespace sese::service::http::v3 {

class HttpServer final {
public:
    template<class CTL, class... ARGS>
    void regController(ARGS &&...args);

    void regMountPoint(const std::string &uri_prefix, const std::string &local);

    void regServlet(const net::http::Servlet::Ptr &servlet);

    void setKeepalive(uint32_t seconds);

    void regService(const net::IPAddress::Ptr &address, const security::SSLContext::Ptr &context);

    void setName(const std::string &name);

    bool startup();

    bool shutdown();

private:
    std::string name;
    uint32_t keepalive = 5;
    std::vector<HttpService::Ptr> services;
    std::vector<net::http::Controller::Ptr> controllers;
    HttpService::MountPointMap mount_points;
    HttpService::ServletMap servlets;
};

template<class CTL, class... ARGS>
void HttpServer::regController(ARGS &&...args) {
    auto controller = std::make_shared<CTL>(std::forward<ARGS>(args)...);
    controller->init();
    for (auto &&servlet: *controller) {
        servlets.emplace(servlet->getUri(), servlet);
    }
}

} // namespace sese::service::http::v3