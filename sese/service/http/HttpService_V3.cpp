#include <sese/service/http/HttpService_V3.h>
#include <sese/internal/service/http/HttpServiceImpl.h>

#include <memory>

sese::service::http::v3::HttpService::Ptr sese::service::http::v3::HttpService::create(
        const net::IPAddress::Ptr &address,
        SSLContextPtr ssl_context,
        uint32_t keepalive,
        std::string &serv_name,
        MountPointMap &mount_points,
        ServletMap &servlets,
        FilterMap &filters
) {
    return std::make_shared<internal::service::http::HttpServiceImpl>(
            address, std::move(ssl_context), keepalive, serv_name, mount_points, servlets, filters
    );
}

sese::service::http::v3::HttpService::HttpService(
        net::IPAddress::Ptr address,
        SSLContextPtr ssl_context,
        uint32_t keepalive,
        std::string &serv_name,
        MountPointMap &mount_points,
        ServletMap &servlets,
        FilterMap &filters
) : address(std::move(address)), ssl_context(std::move(ssl_context)), keepalive(keepalive), serv_name(serv_name), mount_points(mount_points), servlets(servlets), filters(filters) {
}
