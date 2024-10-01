#include <sese/internal/service/http/HttpServiceImpl.h>
#include <sese/service/http/HttpServer.h>
#include <sese/Log.h>
#include <utility>

using namespace sese::service::http;

void HttpServer::regMountPoint(const std::string &uri_prefix, const std::string &local) {
    mount_points[uri_prefix] = local;
}

void HttpServer::regServlet(const net::http::Servlet &servlet) {
    this->servlets.emplace(servlet.getUri(), servlet);
}

void HttpServer::regFilter(const std::string &uri_prefix, const HttpService::FilterCallback &callback) {
    this->filters[uri_prefix] = callback;
}

void HttpServer::setKeepalive(uint32_t seconds) {
    keepalive = std::max<uint32_t>(seconds, 5);
}

void HttpServer::regService(const net::IPAddress::Ptr &address, std::unique_ptr<security::SSLContext> context) {
    auto service = internal::service::http::HttpServiceImpl::create(
        address, std::move(context), keepalive, name, mount_points, servlets, filters, connection_callback
    );
    this->services.push_back(service);
}

void HttpServer::setName(const std::string &name) {
    this->name = name;
}

void HttpServer::setConnectionCallback(const HttpService::ConnectionCallback &callback) {
    this->connection_callback = callback;
}

bool HttpServer::startup() const {
    std::vector<HttpService::Ptr> startup_services;
    for (auto &&item: services) {
        if (item->startup()) {
            SESE_INFO("Service started");
            startup_services.push_back(item);
        } else {
            SESE_ERROR("Failed to startup service: {}", item->getLastError());
            for (auto &&service: startup_services) {
                service->shutdown();
            }
            return false;
        }
    }
    return true;
}

bool HttpServer::shutdown() const {
    for (auto &&item: services) {
        item->shutdown();
    }
    return true;
}
