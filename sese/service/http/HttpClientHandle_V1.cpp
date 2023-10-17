#include <sese/service/http/HttpClientHandle_V1.h>
#include <sese/security/SSLContextBuilder.h>
#include <sese/net/http/RequestParser.h>
#include <sese/util/Util.h>

#include <cassert>

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

    handle->cookies = std::make_shared<net::http::CookieMap>();
    handle->req->setCookies(handle->cookies);
    handle->req->set("client", "sese::service::v1::HttpClient");
    handle->req->set("connection", "keep-alive");

    if (handle->ssl) {
        handle->clientCtx = security::SSLContextBuilder::SSL4Client();
    }

    return handle;
}

sese::service::v1::HttpClientHandle::Ptr sese::service::v1::HttpClientHandle::create(const IPAddress::Ptr &address, const security::SSLContext::Ptr &ctx) {
    assert(address != nullptr);
    auto handle = std::shared_ptr<HttpClientHandle>(new HttpClientHandle);
    handle->ssl = ctx != nullptr;
    handle->address = address;
    handle->req = std::make_unique<Req>();
    handle->resp = std::make_unique<Resp>();

    handle->cookies = std::make_shared<net::http::CookieMap>();
    handle->req->setCookies(handle->cookies);
    handle->req->set("client", "sese::service::v1::HttpClient");
    handle->req->set("connection", "keep-alive");

    handle->clientCtx = ctx;

    return handle;
}

sese::service::v1::HttpClientHandle::RequestStatus sese::service::v1::HttpClientHandle::wait() {
    std::unique_lock lock(mutex);
    conditionVariable.wait(lock);
    lock.unlock();
    return requestStatus;
}

void sese::service::v1::HttpClientHandle::requestForEach(const HeaderForEachFunction &func) const {
    for (auto &iterator: *req) {
        func(iterator);
    }
}

void sese::service::v1::HttpClientHandle::responseForEach(const HeaderForEachFunction &func) const {
    for (auto &iterator: *resp) {
        func(iterator);
    }
}

void sese::service::v1::HttpClientHandle::cookieForEach(const CookieForEachFunction &func) const {
    for (auto &iterator: *cookies) {
        func(iterator.second);
    }
}