// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// @file HttpServer.h
/// @brief HTTP Server
/// @author kaoru
/// @date May 1, 2024

#pragma once

#include <sese/service/http/HttpService.h>
#include <sese/net/http/Controller.h>

namespace sese::service::http {

/// HTTP Server
/// @note Invocation priority: Filter > Mount Point (Filter) > Controller = Servlet, independent and non-convertible
class HttpServer final {
public:
    /// Register controller
    /// @tparam CTL Controller type
    /// @tparam ARGS Instantiation parameter types
    /// @param args Instantiation parameters
    template<class CTL, class... ARGS>
    void regController(ARGS &&...args);

    /// Register file system mount point
    /// @param uri_prefix URI prefix
    /// @param local Local path
    void regMountPoint(const std::string &uri_prefix, const std::string &local);

    /// Register filter
    /// \param uri_prefix URI prefix
    /// \param callback Callback function. If the function returns true, it needs further processing, i.e., continue to determine subsequent mount points, controllers, etc. Otherwise, intercept the current request and respond directly.
    void regFilter(const std::string &uri_prefix, const HttpService::FilterCallback &callback);

    /// Register HTTP application
    /// @param servlet HTTP application
    void regServlet(const net::http::Servlet &servlet);

    /// This method is used to register a post-processing filter that will be executed after other all servlets, controllers, and mount points process exceptions.
    /// If you need to finally modify or process the response (e.g., custom 404 pages), you can use this feature.
    /// The return value indicates whether the interception has been processed. After interception, the response type will change to Controller and accept the relevant processing.
    /// \param tail_filter The post-processing HTTP filter to register, used to handle requests
    void regTailFilter(const HttpService::FilterCallback &tail_filter);

    /// Set keepalive
    /// @param seconds Keepalive duration, minimum value is 5
    void setKeepalive(uint32_t seconds);

    /// Register HTTP service
    /// @param address Listening address
    /// @param context SSL service context, if null, SSL is not enabled
    void regService(const net::IPAddress::Ptr &address, std::unique_ptr<security::SSLContext> context);

    /// Set server name
    /// @param name Server name
    void setName(const std::string &name);

    /// Set connection callback function, the function will be called before the connection object is created, and the SSL connection has not yet been established
    /// @param callback Connection callback function. If the function returns true, normal processing will continue, otherwise the connection will be discarded directly.
    void setConnectionCallback(const HttpService::ConnectionCallback &callback);

    /// Start service
    /// @return Result
    bool startup() const;

    /// Stop service
    /// @return Result
    bool shutdown() const;

private:
    std::string name;
    uint32_t keepalive = 5;
    std::vector<HttpService::Ptr> services;
    std::vector<net::http::Controller::Ptr> controllers;
    HttpService::MountPointMap mount_points;
    HttpService::ServletMap servlets;
    HttpService::FilterMap filters;
    HttpService::FilterCallback tail_filter;
    HttpService::ConnectionCallback connection_callback;
};

template<class CTL, class... ARGS>
void HttpServer::regController(ARGS &&...args) {
    auto controller = std::make_shared<CTL>(std::forward<ARGS>(args)...);
    controller->init();
    for (auto &&servlet: *controller) {
        servlets.emplace(servlet.getUri(), servlet);
    }
    controllers.push_back(controller); // GCOVR_EXCL_LINE
}

} // namespace sese::service::http::v3
