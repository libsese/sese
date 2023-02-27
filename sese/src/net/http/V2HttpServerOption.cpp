#include "sese/net/http/V2HttpServerOption.h"
#include "sese/net/V2Server.h"
#include "sese/net/http/HttpUtil.h"

void sese::net::v2::http::HttpServerOption::onHandle(sese::net::v2::IOContext *ctx) noexcept {
    HttpContext httpContext;
    httpContext.reset(ctx);
    if (!sese::http::HttpUtil::recvRequest(&httpContext, &httpContext.request)) {
        httpContext.close();
        return;
    }

    bool isKeepAlive = this->isKeepAlive &&
                       this->keepAlive > 0 &&
                       0 == strcasecmp(httpContext.request.get("Connection", "Keep-Alive").c_str(), "Keep-Alive");

    onRequest(&httpContext);

    if (!httpContext.isFlushed()) {
        if (!httpContext.flush()) {
            httpContext.close();
            return;
        }
    }

    if (!isKeepAlive) {
        httpContext.close();
        return;
    }
}

void sese::net::v2::http::HttpServerOption::onRequest(sese::net::v2::http::HttpContext *ctx) noexcept {
    const char sendBuffer[] = {"<h1>Default implementation from HttpServerOption</h1>"};
    ctx->response.set("Content-Length", std::to_string(sizeof(sendBuffer) - 1));
    ctx->flush();
    ctx->write(sendBuffer, sizeof(sendBuffer) - 1);
}