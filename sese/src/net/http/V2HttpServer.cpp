#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif


#include "sese/net/http/V2HttpServer.h"
#include "sese/net/V2Server.h"
#include "sese/net/http/HttpUtil.h"
#include "sese/thread/Locker.h"

void sese::net::v2::http::HttpServer::onHandle(sese::net::v2::IOContext &ctx) noexcept {
    HttpContext httpContext;
    httpContext.reset(&ctx);

    if (keepAlive > 0) {
        sese::Locker locker(mutex);
        auto iterator = taskMap.find(ctx.getIdent());
        if (iterator != taskMap.end()) {
            iterator->second->cancel();
        }
    }

    if (!sese::net::http::HttpUtil::recvRequest(&httpContext, &httpContext.request)) {
        httpContext.close();
        return;
    }

    bool isKeepAlive = this->keepAlive > 0 &&
                       0 == strcasecmp(httpContext.request.get("Connection", "Keep-Alive").c_str(), "Keep-Alive");

    onRequest(httpContext);

    if (!httpContext.isFlushed()) {
        if (!httpContext.flush()) {
            httpContext.close();
            return;
        }
    }

    if (!isKeepAlive) {
        httpContext.close();
        return;
    } else {
        auto task = timer->delay(
                [ctx]() {
                    auto myCtx = ctx;
                    myCtx.close();
                },
                (int64_t) keepAlive
        );
        sese::Locker locker(mutex);
        taskMap[ctx.getIdent()] = task;
    }
}

void sese::net::v2::http::HttpServer::onRequest(sese::net::v2::http::HttpContext &ctx) noexcept {
    const char sendBuffer[] = {"<h1>Default implementation from HttpServer</h1>"};
    ctx.response.set("Content-Length", std::to_string(sizeof(sendBuffer) - 1));
    ctx.flush();
    ctx.write(sendBuffer, sizeof(sendBuffer) - 1);
}

//sese::net::v2::http::HttpServerOption::HttpServerOption(size_t keepAlive) noexcept {
//    this->keepAlive = keepAlive;
//    if (keepAlive > 0) {
//        timer = Timer::create();
//    }
//}

bool sese::net::v2::http::HttpServer::init() noexcept {
    if (this->keepAlive > 0) {
        timer = Timer::create();
    }
    return Server::init();
}

sese::net::v2::http::HttpServer::~HttpServer() noexcept {
    if (keepAlive > 0 && timer) {
        timer->shutdown();
    }
}

void sese::net::v2::http::HttpServer::onConnect(sese::net::v2::IOContext &context) noexcept {
    // printf("CONN: %d\n", (int) context.getIdent());
    if (keepAlive > 0) {
        auto task = timer->delay(
                [context]() {
                    auto myCtx = context;
                    myCtx.close();
                },
                (int64_t) keepAlive
        );
        sese::Locker locker(mutex);
        taskMap[context.getIdent()] = task;
    }
}

void sese::net::v2::http::HttpServer::onClosing(sese::net::v2::IOContext &context) noexcept {
    // printf("CLOSE: %d\n", (int) context.getIdent());
    if (keepAlive > 0) {
        sese::Locker locker(mutex);
        auto iterator = taskMap.find(context.getIdent());
        if (iterator != taskMap.end()) {
            iterator->second->cancel();
        }
    }
}

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif