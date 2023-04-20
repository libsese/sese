#include "sese/net/http/V2HttpServerOption.h"
#include "sese/net/V2Server.h"
#include "sese/net/http/HttpUtil.h"
#include "sese/thread/Locker.h"

void sese::net::v2::http::HttpServerOption::onHandle(sese::net::v2::IOContext &ctx) noexcept {
    HttpContext httpContext;
    httpContext.reset(&ctx);

    if (keepAlive > 0) {
        sese::Locker locker(mutex);
        auto iterator = taskMap.find(ctx.getIdent());
        if (iterator != taskMap.end()) {
            iterator->second->cancel();
        }
    }

    if (!sese::http::HttpUtil::recvRequest(&httpContext, &httpContext.request)) {
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

void sese::net::v2::http::HttpServerOption::onRequest(sese::net::v2::http::HttpContext &ctx) noexcept {
    const char sendBuffer[] = {"<h1>Default implementation from HttpServerOption</h1>"};
    ctx.response.set("Content-Length", std::to_string(sizeof(sendBuffer) - 1));
    ctx.flush();
    ctx.write(sendBuffer, sizeof(sendBuffer) - 1);
}

sese::net::v2::http::HttpServerOption::HttpServerOption(size_t keepAlive) noexcept {
    this->keepAlive = keepAlive;
    if (keepAlive > 0) {
        timer = Timer::create();
    }
}

sese::net::v2::http::HttpServerOption::~HttpServerOption() noexcept {
    if (keepAlive > 0) {
        timer->shutdown();
    }
}

void sese::net::v2::http::HttpServerOption::onConnect(sese::net::v2::IOContext &context) noexcept {
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

void sese::net::v2::http::HttpServerOption::onClosing(sese::net::v2::IOContext &context) noexcept {
    // printf("CLOSE: %d\n", (int) context.getIdent());
    if (keepAlive > 0) {
        sese::Locker locker(mutex);
        auto iterator = taskMap.find(context.getIdent());
        if (iterator != taskMap.end()) {
            iterator->second->cancel();
        }
    }
}
