#include <sese/net/http/HttpServer.h>
#include <sese/net/http/HttpUtil.h>
#include "sese/util/Util.h"

using sese::Stream;
using sese::http::HttpServiceContext;
using Server = sese::http::HttpServer;
using sese::http::HttpRequest;
using sese::http::HttpResponse;
using sese::http::HttpUtil;

HttpServiceContext::HttpServiceContext() noexcept {
    request = std::make_unique<HttpRequest>();
    response = std::make_unique<HttpResponse>();
}

void HttpServiceContext::reset(IOContext *context) noexcept {
    ioContext = context;
}

int64_t HttpServiceContext::read(void *buffer, size_t size) noexcept {
    return ioContext->read(buffer, size);
}

int64_t HttpServiceContext::write(const void *buffer, size_t size) noexcept {
    if (!_isFlushed) {
        // try to flush resp
        if (!flush()) {
            return -1;
        }
    }
    return ioContext->write(buffer, size);
}

bool HttpServiceContext::flush() noexcept {
    response->set("Server", HTTPD_NAME);
    if (0 == strcasecmp(request->get("Connection", "Keep-Alive").c_str(), "Close")) {
        response->set("Connection", "Close");
    } else {
        response->set("Connection", "Keep-Alive");
    }

    _isFlushed = true;
    if (!HttpUtil::sendResponse(this, response.get())) {
        _isFlushed = false;
        return false;
    } else {
        return true;
    }
}

void sese::http::HttpServiceContext::close() {
    ioContext->close();
}

Server::Ptr Server::create(const IPAddress::Ptr &ipAddress, size_t threads, size_t keepAlive) noexcept {
    auto tcpServer = TcpServer::create(ipAddress, threads, keepAlive);
    if (nullptr == tcpServer) {
        return nullptr;
    } else {
        auto server = new Server;
        server->tcpServer = std::move(tcpServer);
        return std::unique_ptr<Server>(server);
    }
}

void Server::loopWith(const std::function<void(HttpServiceContext *)> &handler) {
    tcpServer->loopWith([&](IOContext *ioContext) {
        HttpServiceContext context;
        context.reset(ioContext);

        decltype(auto) request = context.getRequest();
        decltype(auto) response = context.getResponse();

        if (!HttpUtil::recvRequest(&context, context.getRequest().get())) {
            context.close();
            return;
        }

        bool isKeepAlive = tcpServer->getKeepAlive() != 0 && 0 == strcasecmp(request->get("Connection", "Keep-Alive").c_str(), "Keep-Alive");

        handler(&context);

        if (!context.isFlushed()) {
            if (!context.flush()) {
                context.close();
                return;
            }
        }

        if (!isKeepAlive) {
            context.close();
            return;
        }
    });
}

void Server::shutdown() {
    tcpServer->shutdown();
}