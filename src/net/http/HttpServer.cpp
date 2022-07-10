#include <sese/net/http/HttpServer.h>
#include <sese/net/http/HttpUtil.h>

using sese::IOContext;
using sese::TcpServer;
using sese::http::HttpRequest;
using sese::http::HttpResponse;
using sese::http::HttpServer;
using sese::http::HttpServiceContext;
using sese::http::HttpUtil;

HttpServiceContext::HttpServiceContext() noexcept {
    request = std::make_unique<HttpRequest>();
    response = std::make_unique<HttpResponse>();
}

void HttpServiceContext::reset(IOContext *ioContext) noexcept {
    this->ioContext = ioContext;
    isReadOnly = true;
}

int64_t HttpServiceContext::read(void *buffer, size_t size) noexcept {
    if (isReadOnly) {
        return ioContext->read(buffer, size);
    } else {
        return -1;
    }
}

int64_t HttpServiceContext::write(const void *buffer, size_t size) noexcept {
    if (isReadOnly) {
        return -1;
    } else {
        return ioContext->write(buffer, size);
    }
}

bool HttpServiceContext::flush() noexcept {
    isReadOnly = false;
    return HttpUtil::sendResponse(ioContext, response.get());
}

HttpServer::Ptr HttpServer::create(const IPAddress::Ptr &ipAddress, size_t threads) noexcept {
    HttpServer::Ptr server = std::unique_ptr<HttpServer>(new HttpServer);
    server->tcpServer = TcpServer::create(ipAddress, threads);
    if (server->tcpServer == nullptr) {
        return nullptr;
    }
    server->objectPool = ObjectPool<HttpServiceContext>::create();
    return server;
}

void HttpServer::loopWith(std::function<void(const HttpServiceContext::Ptr &)> handler) {
    tcpServer->loopWith([&](IOContext *ioContext) {
        ObjectPool<HttpServiceContext>::ObjectPtr context = objectPool->borrow();

        decltype(auto) request = context->getRequest();
        if(HttpUtil::recvRequest(ioContext, request.get())) {
            context->reset(ioContext);
            handler(context);
        }
    });
}

void HttpServer::shutdown() {
    tcpServer->shutdown();
}