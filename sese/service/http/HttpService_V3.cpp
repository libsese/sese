#include <sese/service/http/HttpService_V3.h>
#include <sese/internal/service/http/HttpServiceImpl_V3.h>

#include <memory>

sese::service::http::v3::HttpService::Ptr sese::service::http::v3::HttpService::create() {
    return std::make_shared<internal::service::http::v3::HttpServiceImpl>();
}
