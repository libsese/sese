#include <sese/net/http/HttpServer.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/Util.h>

#define CLOSE(CONTENT)                             \
    CONTENT->shutdown(Socket::ShutdownMode::Both); \
    CONTENT->close()

#ifdef _WIN32
/// \see flush_windows_sock_sending_buffer
static char SPACE_BUFFER[64]{};
#endif

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
    server->timer = Timer::create();
    server->objectPool = concurrent::ConcurrentObjectPool<HttpServiceContext>::create();
    return server;
}

void HttpServer::loopWith(const std::function<void(const HttpServiceContext::Ptr &)> &handler) {
    tcpServer->loopWith([&](IOContext *ioContext) {
        concurrent::ConcurrentObjectPool<HttpServiceContext>::ObjectPtr context = objectPool->borrow();
        context->reset(ioContext);

        decltype(auto) request = context->getRequest();
        request->set("Server", HTTPD_NAME);
        bool noKeepAlive = 0 == strcasecmp(request->get("Connection", "keep-alive").c_str(), "close");
        if (!noKeepAlive) {
            // Connection need keep alive
            // Add to timing task
            // ...
            auto socket = ioContext->getSocket();
            auto task = timer->delay(std::bind(&HttpServer::closeCallback, taskMap, socket), HTTPD_KEEP_ALIVE_DURATION, false);
            mutex.lock();
            auto iterator = taskMap.find(socket);
            if (iterator == taskMap.end()) {
                // It's a new connection,
                // add to taskMap as TimerTask
                taskMap[socket] = task;
            } else {
                // It's an old connection,
                // reset TimerTask timing count,
                auto originalTask = iterator->second;
                // Cancel the original task from timer,
                // and set a new task
                originalTask->cancel();
                iterator->second = task;
            }
            mutex.unlock();
        }

        if (!HttpUtil::recvRequest(ioContext, request.get())) {
            CLOSE(ioContext);
            return;
        }
        handler(context);
        if (context->isReadOnly()) {
            if (!context->flush()) {
                CLOSE(ioContext);
                return;
            }
        }

        if (noKeepAlive) {
            // Connection need close
            CLOSE(ioContext);
        }
#ifdef _WIN32
        else {
            /// \anchor flush_windows_sock_sending_buffer
            /// \warning Do not remove this!
            /// \brief Send 64 bytes blank char to force flash the windows socket sending buffer.
            ioContext->write(SPACE_BUFFER, 64);
        }
#endif
    });
}

void HttpServer::shutdown() {
    tcpServer->shutdown();
    timer->shutdown();
    for (const auto &pair: taskMap) {
        CLOSE(pair.first);
    }
}

void HttpServer::closeCallback(Map &taskMap, const Socket::Ptr &socket) noexcept {
    taskMap.erase(socket);
    CLOSE(socket);
}

#undef CLOSE