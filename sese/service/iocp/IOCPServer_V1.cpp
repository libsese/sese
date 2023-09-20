#include <sese/service/iocp/IOCPServer_V1.h>

#include <openssl/ssl.h>

using namespace sese::iocp;
using namespace sese::service;

#pragma region Service

IOCPService_V1::IOCPService_V1(IOCPServer_V1 *master)
    : TimerableService_V2() {
    IOCPService_V1::master = master;
}

void IOCPService_V1::postRead(IOCPService_V1::Context *ctx) {
    ctx->event->events |= EVENT_READ;
    IOCPService_V1::setEvent(ctx->event);
}

void IOCPService_V1::postWrite(IOCPService_V1::Context *ctx) {
    ctx->event->events |= EVENT_WRITE;
    IOCPService_V1::setEvent(ctx->event);
}

void IOCPService_V1::onReadCompleted(IOCPService_V1::Context *ctx) {
    ctx->self->onReadCompleted(ctx);
}

void IOCPService_V1::onWriteCompleted(IOCPService_V1::Context *ctx) {
    ctx->self->onWriteCompleted(ctx);
}

void IOCPService_V1::onTimeout(IOCPService_V1::Context *ctx) {
    ctx->self->onTimeout(ctx);
}

// todo 实现 IOCP 核心功能
void IOCPService_V1::onAccept(int fd) {
}

void IOCPService_V1::onRead(sese::event::BaseEvent *event) {
}

void IOCPService_V1::onWrite(sese::event::BaseEvent *event) {
}

void IOCPService_V1::onTimeout(TimeoutEvent_V2 *event) {
    auto ctx = (Context *) event->data;
    IOCPService_V1::onTimeout(ctx);
}

int IOCPService_V1::alpnCallbackFunction([[maybe_unused]] void *ssl, const uint8_t **out, uint8_t *outLength, const uint8_t *in, uint32_t inLength, IOCPService_V1 *service) {
    return service->master->onAlpnSelect(out, outLength, in, inLength);
}

#pragma endregion Service

#pragma region Server

void IOCPServer_V1::postRead(IOCPServer_V1::Context *ctx) {
    ctx->client->postRead(ctx);
}

void IOCPServer_V1::postWrite(IOCPService_V1::Context *ctx) {
    ctx->client->postWrite(ctx);
}

void IOCPServer_V1::setTimeout(IOCPServer_V1::Context *ctx, int64_t seconds) {
    if (ctx->timeoutEvent) {
        ctx->client->cancelTimeoutEvent(ctx->timeoutEvent);
    }
    ctx->timeoutEvent = ctx->client->setTimeoutEvent(seconds, ctx);
    ctx->timeoutEvent->data = ctx;
}

void IOCPServer_V1::cancelTimeout(IOCPServer_V1::Context *ctx) {
    if (ctx->timeoutEvent) {
        ctx->client->cancelTimeoutEvent(ctx->timeoutEvent);
        ctx->timeoutEvent = nullptr;
    }
}

int IOCPServer_V1::onAlpnSelect(const uint8_t **out, uint8_t *outLength, const uint8_t *in, uint32_t inLength) {
    if (SSL_select_next_proto((unsigned char **) out, outLength, (const uint8_t *) servProtos.c_str(), servProtos.length(), in, inLength) != OPENSSL_NPN_NEGOTIATED) {
        return SSL_TLSEXT_ERR_NOACK;
    }
    return SSL_TLSEXT_ERR_OK;
}

#pragma endregion Service