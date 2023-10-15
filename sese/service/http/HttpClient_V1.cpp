#include <sese/service/http/HttpClient_V1.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/util/Util.h>

sese::service::v1::HttpClient::HttpClient() {
    setDeleteContextCallback(deleter);
}

void sese::service::v1::HttpClient::post(const HttpClientHandle::Ptr &handle) {
    if (handle->writeRequestBodyCallback) {
        char *endPtr;
        handle->requestBodySize = std::strtoul(handle->req->get("content-length", "0").c_str(), &endPtr, 10);
    } else {
        handle->requestBodySize = handle->req->getBody().getReadableSize();
        handle->req->set("content-length", std::to_string(handle->requestBodySize));
    }

    auto data = new Data;
    data->handle = handle;
    Supper::postConnect(handle->address, handle->clientCtx, data);
}

void sese::service::v1::HttpClient::deleter(sese::iocp::Context *ctx) {
    auto data = static_cast<struct Data *>(ctx->getData());
    delete data;
}

void sese::service::v1::HttpClient::onPreRead(Context *ctx) {
    cancelTimeout(ctx);
}

void sese::service::v1::HttpClient::onReadCompleted(Context *ctx) {
    auto data = static_cast<struct Data *>(ctx->getData());
    auto handle = data->handle;
    char ch;
    if (0 == ctx->peek(&ch, sizeof(ch))) {
        setTimeout(ctx, static_cast<int64_t>(handle->responseTimeout));
        postRead(ctx);
        return;
    }

    if (handle->responseBodySize == 0) {
        auto parseRt = net::http::HttpUtil::recvResponse(ctx, handle->resp.get());
        if (!parseRt) {
            postClose(ctx);
            return;
        }
    }

    char *endPtr;
    handle->responseBodySize = std::strtoul(handle->resp->get("content-length", "0").c_str(), &endPtr, 10);
    if (handle->responseBodySize) {
        while (handle->responseBodySize - handle->responseBodyHandled) {
            char buffer[MTU_VALUE];
            auto need = std::min<size_t>(handle->responseBodySize - handle->responseBodyHandled, MTU_VALUE);
            auto l = ctx->read(buffer, need);
            if (l) {
                handle->responseBodyHandled += l;
                if (handle->readResponseBodyCallback) {
                    handle->readResponseBodyCallback(buffer, l);
                } else {
                    handle->resp->getBody().write(buffer, l);
                }
            } else {
                setTimeout(ctx, static_cast<int64_t>(handle->responseTimeout));
                postRead(ctx);
                return;
            }
        }
    }
    handle->requestStatus = HttpClientHandle::RequestStatus::Succeeded;
    if (handle->requestDoneCallback) {
        handle->requestDoneCallback(handle);
    }
    handle->conditionVariable.notify_all();
}

void sese::service::v1::HttpClient::onWriteCompleted(Context *ctx) {
    cancelTimeout(ctx);
    auto data = static_cast<struct Data *>(ctx->getData());
    auto handle = data->handle;
    if (handle->requestBodySize - handle->requestBodyHandled > 0) {
        if (handle->writeRequestBodyCallback) {
            size_t size;
            handle->writeRequestBodyCallback(ctx, &size);
            handle->requestBodyHandled -= std::min<size_t>(size, handle->requestBodyHandled);
        } else {
            auto &&body = handle->req->getBody();
            auto size = sese::streamMove(ctx, &body, MTU_VALUE);
            handle->requestBodyHandled -= std::min<size_t>(size, handle->requestBodyHandled);
        }
        setTimeout(ctx, static_cast<int64_t>(handle->requestTimeout));
        postWrite(ctx);
    } else {
        handle->requestStatus = HttpClientHandle::RequestStatus::Responding;
        setTimeout(ctx, static_cast<int64_t>(handle->responseTimeout));
        postRead(ctx);
    }
}

void sese::service::v1::HttpClient::onTimeout(Context *ctx) {
    postClose(ctx);
}

void sese::service::v1::HttpClient::onPreConnect(Context *ctx) {
    auto data = static_cast<struct Data *>(ctx->getData());
    auto handle = data->handle;
    handle->requestStatus = HttpClientHandle::RequestStatus::Connecting;
    setTimeout(ctx, static_cast<int64_t>(handle->connectTimeout));
}

void sese::service::v1::HttpClient::onConnected(Context *ctx) {
    cancelTimeout(ctx);
    auto data = static_cast<struct Data *>(ctx->getData());
    auto handle = data->handle;
    handle->requestStatus = HttpClientHandle::RequestStatus::Requesting;
    net::http::HttpUtil::sendRequest(ctx, handle->req.get());
    setTimeout(ctx, static_cast<int64_t>(handle->requestTimeout));
    postWrite(ctx);
}