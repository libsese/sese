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

#include <sese/service/http/HttpService.h>
#include <sese/internal/service/http/HttpServiceImpl.h>

#include <memory>

sese::service::http::HttpService::Ptr sese::service::http::HttpService::create(
        const net::IPAddress::Ptr &address,
        SSLContextPtr ssl_context,
        uint32_t keepalive,
        std::string &serv_name,
        MountPointMap &mount_points,
        ServletMap &servlets,
        FilterCallback &tail_filter,
        FilterMap &filters,
        ConnectionCallback &connection_callback
) {
    return std::make_shared<internal::service::http::HttpServiceImpl>(
            address,
            std::move(ssl_context),
            keepalive,
            serv_name,
            mount_points,
            servlets,
            tail_filter,
            filters,
            connection_callback
    );
}

sese::service::http::HttpService::HttpService(
        net::IPAddress::Ptr address,
        SSLContextPtr ssl_context,
        uint32_t keepalive,
        std::string &serv_name,
        MountPointMap &mount_points,
        ServletMap &servlets,
        FilterCallback &tail_filter,
        FilterMap &filters,
        ConnectionCallback &connection_callback
) : address(std::move(address)),
    ssl_context(std::move(ssl_context)),
    keepalive(keepalive),
    serv_name(serv_name),
    mount_points(mount_points),
    servlets(servlets),
    tail_filter(tail_filter),
    filters(filters),
    connection_callback(connection_callback) {
}
