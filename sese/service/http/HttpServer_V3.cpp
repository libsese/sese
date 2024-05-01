#include <sese/internal/service/http/HttpServiceImpl_V3.h>
#include <sese/service/http/HttpServer_V3.h>

using namespace sese::service::http::v3;

void HttpServer::regMountPoint(const std::string &uri_prefix, const std::string &local) {
    mount_points[uri_prefix] = local;
}

void HttpServer::regServlet(const net::http::Servlet &servlet) {
    this->servlets.emplace(servlet.getUri(), servlet);
}

void HttpServer::setKeepalive(uint32_t seconds) {
    keepalive = std::min<uint32_t>(seconds, 5);
}

void HttpServer::regService(const net::IPAddress::Ptr &address, const security::SSLContext::Ptr &context) {
    auto service = internal::service::http::v3::HttpServiceImpl::create(
            address, context, keepalive, name, mount_points, servlets
    );
    this->services.push_back(service);
}

void HttpServer::setName(const std::string &name) {
    this->name = name;
}

bool HttpServer::startup() {
    for (auto &&item: services) {
        item->startup();
    }
    return true;
}

bool HttpServer::shutdown() {
    for (auto &&item: services) {
        item->shutdown();
    }
    return true;
}
