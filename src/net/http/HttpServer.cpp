#include <sese/net/http/HttpServer.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/Util.h>

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

void HttpServiceContext::reset(IOContext *context) noexcept {
    ioContext = context;
    _isReadOnly = true;
    request->clear();
    response->clear();
}

int64_t HttpServiceContext::read(void *buffer, size_t size) noexcept {
    if (_isReadOnly) {
        return ioContext->read(buffer, size);
    } else {
        return -1;
    }
}

int64_t HttpServiceContext::write(const void *buffer, size_t size) noexcept {
    if (_isReadOnly) {
        // try switch to write mode
        if (!flush()) {
            return -1;
        }
    }
    return ioContext->write(buffer, size);
}

bool HttpServiceContext::flush() noexcept {
    if (0 == strcasecmp(request->get("Connection", "keep-alive").c_str(), "close")) {
        response->set("Connection", "close");
    } else {
        response->set("Connection", "keep-alive");
    }
    auto rt = HttpUtil::sendResponse(ioContext, response.get());
    if (rt) {
        _isReadOnly = false;
    }
    return rt;
}

HttpServer::Ptr HttpServer::create(const IPAddress::Ptr &ipAddress, size_t threads) noexcept {
    HttpServer::Ptr server = std::unique_ptr<HttpServer>(new HttpServer);
    server->tcpServer = TcpServer::create(ipAddress, threads);
    if (server->tcpServer == nullptr) {
        return nullptr;
    }
    server->objectPool = concurrent::ConcurrentObjectPool<HttpServiceContext>::create();
    server->timer = std::make_shared<Timer>();
    return server;
}

void HttpServer::loopWith(std::function<void(const HttpServiceContext::Ptr &)> handler) {
    tcpServer->loopWith([&](IOContext *ioContext) {
        concurrent::ConcurrentObjectPool<HttpServiceContext>::ObjectPtr context = objectPool->borrow();
        context->reset(ioContext);

        decltype(auto) request = context->getRequest();
        decltype(auto) response = context->getResponse();
        response->set("Server", HTTPD_NAME);

        if (!HttpUtil::recvRequest(ioContext, request.get())) {
            ioContext->shutdown(Socket::ShutdownMode::Both);
            ioContext->close();
            return;
        }

        handler(context);
        bool noKeepAlive = 0 == strcasecmp(request->get("Connection", "keep-alive").c_str(), "close");
        if (noKeepAlive) {
            // Connection need close
            ioContext->shutdown(Socket::ShutdownMode::Both);
            ioContext->close();
        } else {
            // Connection need keep alive
            // Add to timing task
            // ...
        }
    });
}

void HttpServer::shutdown() {
    tcpServer->shutdown();
}