#include <sese/service/http/HttpClient_V1.h>
#include <sese/text/DateTimeFormatter.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/util/Util.h>
#include <sese/record/Marco.h>

sese::service::v1::HttpClient::HttpClient() {
    Supper::setDeleteContextCallback(deleter);
    Supper::setActiveReleaseMode(false);
    Supper::clientProtos = "\x8http/1.1";
}

void sese::service::v1::HttpClient::post(const HttpClientHandle::Ptr &handle) {
    auto time = DateTime::now(0);
    auto timeString = text::DateTimeFormatter::format(time, TIME_GREENWICH_MEAN_PATTERN);
    handle->req->set("date", timeString);

    handle->responseBodySize = 0;
    handle->responseBodyHandled = 0;
    handle->resp->clear();
    handle->cookies->expired(time.getTimestamp() / 1000 / 1000);

    if (handle->writeRequestBodyCallback) {
        // char *endPtr;
        // handle->requestBodySize = std::strtoul(handle->req->get("content-length", "0").c_str(), &endPtr, 10);
    } else {
        handle->requestBodySize = handle->req->getBody().getReadableSize();
    }
    handle->req->set("content-length", std::to_string(handle->requestBodySize));

    handle->requestStatus = HttpClientHandle::RequestStatus::Ready;
    if (handle->context == nullptr) {
        auto data = new Data;
        data->handle = handle;
        Supper::postConnect(handle->address, handle->clientCtx, data);
    } else {
        onConnected(handle->context);
    }
}

void sese::service::v1::HttpClient::deleter(sese::iocp::Context *ctx) {
    auto data = static_cast<struct Data *>(ctx->getData());
    auto handle = data->handle;
    handle->context = nullptr;
    if (handle->requestStatus != HttpClientHandle::RequestStatus::Succeeded) {
        if (handle->requestStatus == HttpClientHandle::RequestStatus::Connecting) {
            handle->requestStatus = HttpClientHandle::RequestStatus::ConnectFailed;
        } else if (handle->requestStatus == HttpClientHandle::RequestStatus::Requesting) {
            handle->requestStatus = HttpClientHandle::RequestStatus::RequestFailed;
        } else if (handle->requestStatus == HttpClientHandle::RequestStatus::Responding) {
            handle->requestStatus = HttpClientHandle::RequestStatus::ResponseFailed;
        }
        if (handle->requestDoneCallback) {
            handle->requestDoneCallback(handle);
        }
        handle->conditionVariable.notify_all();
    }
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
    auto newCookies = handle->resp->getCookies();
    if (newCookies && !newCookies->empty()) {
        auto dateString = handle->resp->get("date", {});
        if (!dateString.empty()) {
            auto date = text::DateTimeFormatter::parseFromGreenwich(dateString);
            if (date) {
                for (decltype(auto) iterator = newCookies->begin(); iterator != newCookies->end(); ++iterator) {
                    iterator->second->update(date);
                    handle->cookies->add(iterator->second);
                }
            }
        }
    }
    handle->requestStatus = HttpClientHandle::RequestStatus::Succeeded;
    if (handle->requestDoneCallback) {
        handle->requestDoneCallback(handle);
    }
    handle->conditionVariable.notify_all();
    auto keepalive = sese::strcmpDoNotCase(handle->resp->get("connection", "").c_str(), "keep-alive");
    if (!keepalive) {
        postClose(ctx);
    }
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
    auto data = static_cast<struct Data *>(ctx->getData());
    auto handle = data->handle;
    handle->context = nullptr;
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
    handle->context = ctx;
    handle->requestStatus = HttpClientHandle::RequestStatus::Requesting;
    net::http::HttpUtil::sendRequest(ctx, handle->req.get());
    setTimeout(ctx, static_cast<int64_t>(handle->requestTimeout));
    postWrite(ctx);
}