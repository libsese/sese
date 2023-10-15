#include <sese/service/http/HttpClientHandle_V1.h>
#include <sese/security/SSLContextBuilder.h>
#include <sese/net/http/RequestParser.h>
#include <sese/util/Util.h>

sese::service::v1::HttpClientHandle::Ptr sese::service::v1::HttpClientHandle::create(const std::string &url) {
    auto result = sese::net::http::RequestParser::parse(url);
    if (result.address == nullptr) {
        return nullptr;
    }

    auto handle = std::shared_ptr<HttpClientHandle>(new HttpClientHandle);
    handle->ssl = sese::strcmpDoNotCase("https", result.url.getProtocol().c_str());
    handle->address = std::move(result.address);
    handle->req = std::move(result.request);
    handle->resp = std::make_unique<Resp>();

    handle->req->set("client", "sese::service::v1::HttpClient");

    if (handle->ssl) {
        handle->clientCtx = security::SSLContextBuilder::SSL4Client();
    }

    return handle;
}

const sese::net::http::Request::Ptr &sese::service::v1::HttpClientHandle::getReq() const {
    return req;
}

const sese::net::http::Response::Ptr &sese::service::v1::HttpClientHandle::getResp() const {
    return resp;
}

sese::service::v1::HttpClientHandle::RequestStatus sese::service::v1::HttpClientHandle::wait() {
    std::unique_lock lock(mutex);
    conditionVariable.wait(lock);
    lock.unlock();
    return requestStatus;
}
