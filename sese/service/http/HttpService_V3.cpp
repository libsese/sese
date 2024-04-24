#include <sese/service/http/HttpService_V3.h>
#include <sese/internal/service/http/HttpServiceImpl_V3.h>

sese::service::http::v3::HttpSerivce::Ptr sese::service::http::v3::HttpSerivce::create() {
    return std::make_shared<internal::service::http::v3::HttpServiceImpl>();
}
