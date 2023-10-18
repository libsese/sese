#include <sese/service/iocp/IOCPServer_V1.h>

#include <openssl/ssl.h>

using namespace sese::iocp::v1;
using namespace sese::service;

#pragma region Context

int64_t Context::read(void *buffer, size_t length) {
    return recv.read(buffer, length);
}

int64_t Context::write(const void *buffer, size_t length) {
    return send.write(buffer, length);
}

int64_t Context::peek(void *buffer, size_t length) {
    return recv.peek(buffer, length);
}

int64_t Context::trunc(size_t length) {
    return recv.trunc(length);
}

#pragma endregion Context

#pragma region Service

IOCPService::IOCPService(IOCPServer *master, bool activeReleaseMode)
    : TimerableService() {
    IOCPService::master = master;
    IOCPService::handleClose = activeReleaseMode;

    if (master->getServCtx()) {
        auto serverSSL = (SSL_CTX *) master->getServCtx()->getContext();
        SSL_CTX_set_alpn_select_cb(
                serverSSL,
                (SSL_CTX_alpn_select_cb_func) &alpnCallbackFunction,
                this
        );
    }
}

IOCPService::~IOCPService() {
    for (auto &&item: eventSet) {
        auto ctx = (Context *) item->data;
        event::EventLoop::freeEvent(item);
        if (ctx->timeoutEvent) {
            cancelTimeoutEvent(ctx->timeoutEvent);
        }
        ctx->self->getDeleteContextCallback()(ctx);
        delete ctx;
    }
}

void IOCPService::postRead(Context *ctx) {
    if (ctx->event) {
        ctx->event->events &= ~EVENT_WRITE;
        ctx->event->events |= EVENT_READ;
        IOCPService::setEvent(ctx->event);
    } else {
        ctx->event = createEventEx((int) ctx->fd, EVENT_READ, ctx);
        ctx->event->data = ctx;
    }
}

void IOCPService::postWrite(Context *ctx) {
    if (ctx->event) {
        ctx->event->events &= ~EVENT_READ;
        ctx->event->events |= EVENT_WRITE;
        IOCPService::setEvent(ctx->event);
    } else {
        ctx->event = createEventEx((int) ctx->fd, EVENT_WRITE, ctx);
        ctx->event->data = ctx;
    }
}

void IOCPService::postClose(Context *ctx) {
    if (handleClose) {
        using namespace sese::net;
        Socket::shutdown(ctx->fd, Socket::ShutdownMode::Both);
    } else {
        releaseContext(ctx);
    }
}

void IOCPService::onAcceptCompleted(Context *ctx) {
    ctx->self->onAcceptCompleted(ctx);
}

void IOCPService::onPreRead(Context *ctx) {
    ctx->self->onPreRead(ctx);
}

void IOCPService::onReadCompleted(Context *ctx) {
    ctx->self->onReadCompleted(ctx);
}

void IOCPService::onWriteCompleted(Context *ctx) {
    ctx->self->onWriteCompleted(ctx);
}

void IOCPService::onTimeout(Context *ctx) {
    ctx->timeoutEvent = nullptr;
    ctx->self->onTimeout(ctx);
}

void IOCPService::onConnected(Context *ctx) {
    ctx->self->onConnected(ctx);
}

void IOCPService::onAlpnGet(Context *ctx, const uint8_t *in, uint32_t inLength) {
    ctx->self->onAlpnGet(ctx, in, inLength);
}

void IOCPService::onAccept(int fd) {
    SSL *clientSSL;
    if (sese::net::Socket::setNonblocking(fd)) {
        sese::net::Socket::close(fd);
    }

    auto ctx = new Context;
    ctx->self = master;
    ctx->client = this;
    ctx->fd = fd;

    if (master->getServCtx()) {
        auto serverSSL = (SSL_CTX *) master->getServCtx()->getContext();
        clientSSL = SSL_new(serverSSL);
        SSL_set_fd(clientSSL, fd);
        SSL_set_accept_state(clientSSL);

        while (true) {
            auto rt = SSL_do_handshake(clientSSL);
            if (rt <= 0) {
                auto err = SSL_get_error(clientSSL, rt);
                if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                    SSL_free(clientSSL);
                    sese::net::Socket::close(fd);
                    delete ctx;
                    return;
                }
            } else {
                ctx->ssl = clientSSL;
                SSL_set_mode(clientSSL, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
                const uint8_t *data = nullptr;
                uint32_t dataLength;
                SSL_get0_alpn_selected(clientSSL, &data, &dataLength);
                onAlpnGet(ctx, data, dataLength);
                break;
            }
        }
    }

    onAcceptCompleted(ctx);
}

void IOCPService::onRead(sese::event::BaseEvent *event) {
    auto ctx = (Context *) event->data;

    onPreRead(ctx);

    size_t len = 0;
    char buf[MTU_VALUE];
    while (true) {
        auto l = read((int) ctx->fd, buf, MTU_VALUE, ctx->ssl);
        if (l <= 0) {
            if (l == 0 && len == 0) {
                releaseContext(ctx);
                break;
            } else {
                onReadCompleted(ctx);
                break;
            }
        } else {
            ctx->recv.write(buf, l);
            len += static_cast<size_t>(l);
        }
    }
}

void IOCPService::onWrite(sese::event::BaseEvent *event) {
    auto ctx = (Context *) event->data;
    if (ctx->isConn) {
        ctx->isConn = false;
        auto ssl = (SSL *) ctx->ssl;
        if (ctx->ssl) {
            SSL_set_connect_state(ssl);
            // GCOVR_EXCL_START
            while (true) {
                auto rt = SSL_do_handshake((SSL *) ssl);
                if (rt <= 0) {
                    // err is SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE
                    auto err = SSL_get_error((SSL *) ssl, rt);
                    if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                        SSL_free((SSL *) ssl);
                        ctx->ssl = nullptr;
                        ssl = nullptr;
                        break;
                    }
                } else {
                    break;
                }
            }
            // GCOVR_EXCL_STOP
            if (ssl == nullptr) {
                releaseContext(ctx);
                return;
            } else {
                SSL_set_mode(ssl, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
                const uint8_t *data = nullptr;
                uint32_t dataLength;
                SSL_get0_alpn_selected(ssl, &data, &dataLength);
                onAlpnGet(ctx, data, dataLength);
            }
        }
        onConnected(ctx);
    } else {
        char buf[MTU_VALUE];
        while (true) {
            auto len = ctx->send.peek(buf, MTU_VALUE);
            if (len == 0) {
                ctx->send.freeCapacity();
                onWriteCompleted(ctx);
                break;
            }
            auto l = write((int) ctx->fd, buf, len, ctx->ssl);
            if (l <= 0) {
                auto err = sese::net::getNetworkError();
                if (err == EWOULDBLOCK || err == EINTR) {
                    postWrite(ctx);
                    break;
                } else {
                    releaseContext(ctx);
                    break;
                }
            } else {
                ctx->send.trunc(l);
            }
        }
    }
}

void IOCPService::onClose(sese::event::BaseEvent *event) {
    auto ctx = (Context *) event->data;
    releaseContext(ctx);
}

void IOCPService::onTimeout(v2::TimeoutEvent *event) {
    auto ctx = (Context *) event->data;
    IOCPService::onTimeout(ctx);
}

int IOCPService::alpnCallbackFunction([[maybe_unused]] void *ssl, const uint8_t **out, uint8_t *outLength, const uint8_t *in, uint32_t inLength, IOCPService *service) {
    return service->master->onAlpnSelect(out, outLength, in, inLength);
}

sese::event::BaseEvent *IOCPService::createEventEx(int fd, unsigned int events, void *data) {
    auto event = createEvent(fd, events, data);
    eventSet.emplace(event);
    return event;
}

void IOCPService::freeEventEx(sese::event::BaseEvent *event) {
    eventSet.erase(event);
    freeEvent(event);
}

void IOCPService::releaseContext(Context *ctx) {
    if (ctx->ssl) {
        SSL_free((SSL *) ctx->ssl);
        ctx->ssl = nullptr;
    }
    if (ctx->timeoutEvent) {
        cancelTimeoutEvent(ctx->timeoutEvent);
    }
    sese::net::Socket::close(ctx->fd);
    ctx->client->freeEventEx(ctx->event);
    ctx->self->getDeleteContextCallback()(ctx);
    delete ctx;
}

int64_t IOCPService::read(int fd, void *buffer, size_t len, void *ssl) {
    if (ssl) {
        return SSL_read((SSL *) ssl, buffer, (int) len);
    } else {
        return sese::net::Socket::read(fd, buffer, len, 0);
    }
}

int64_t IOCPService::write(int fd, const void *buffer, size_t len, void *ssl) {
    if (ssl) {
        return SSL_write((SSL *) ssl, buffer, (int) len);
    } else {
        return sese::net::Socket::write(fd, buffer, len, 0);
    }
}

#pragma endregion Service

#pragma region Server

IOCPServer::IOCPServer() {
    this->balanceLoader.setOnDispatchedCallbackFunction(
            [&](int fd, sese::event::EventLoop *eventLoop, void *data) {
                this->preConnectCallback(fd, eventLoop, (Context *) data);
            }
    );
}

bool IOCPServer::init() {
    auto rt = balanceLoader.init<IOCPService>([this]() -> auto {
        return new IOCPService(this, this->activeReleaseMode);
    });
    if (rt) {
        balanceLoader.start();
        return true;
    }
    return false;
}

void IOCPServer::shutdown() {
    balanceLoader.stop();
}

void IOCPServer::postRead(Context *ctx) {
    ctx->client->postRead(ctx);
}

void IOCPServer::postWrite(Context *ctx) {
    ctx->client->postWrite(ctx);
}

void IOCPServer::postClose(Context *ctx) {
    ctx->client->postClose(ctx);
}

void IOCPServer::postConnect(const net::IPAddress::Ptr &to, const security::SSLContext::Ptr &cliCtx, void *data) {
    auto sock = sese::net::Socket::socket(to->getFamily(), SOCK_STREAM, IPPROTO_IP);
    sese::net::Socket::setNonblocking(sock);

    auto rt = connect(sock, to->getRawAddress(), to->getRawAddressLength());
    if (rt) {
        auto err = sese::net::getNetworkError();
        if (err != EINPROGRESS) {
            return;
        }
    }

    auto ctx = new Context;
    ctx->fd = sock;
    ctx->self = this;
    ctx->isConn = true;
    ctx->data = data;
    if (cliCtx) {
        ctx->ssl = SSL_new((SSL_CTX *) cliCtx->getContext());
        SSL_set_fd((SSL *) ctx->ssl, (int) sock);
        SSL_set_alpn_protos((SSL *) ctx->ssl, (const unsigned char *) clientProtos.c_str(), (unsigned) clientProtos.length());
    }

    balanceLoader.dispatchSocket(sock, ctx);
}

void IOCPServer::setTimeout(Context *ctx, int64_t seconds) {
    if (ctx->timeoutEvent) {
        ctx->client->cancelTimeoutEvent(ctx->timeoutEvent);
        ctx->timeoutEvent = nullptr;
    }
    ctx->timeoutEvent = ctx->client->setTimeoutEvent(seconds, ctx);
    ctx->timeoutEvent->data = ctx;
}

void IOCPServer::cancelTimeout(Context *ctx) {
    if (ctx->timeoutEvent) {
        ctx->client->cancelTimeoutEvent(ctx->timeoutEvent);
        ctx->timeoutEvent = nullptr;
    }
}

int IOCPServer::onAlpnSelect(const uint8_t **out, uint8_t *outLength, const uint8_t *in, uint32_t inLength) {
    if (SSL_select_next_proto((unsigned char **) out, outLength, (const uint8_t *) servProtos.c_str(), (int) servProtos.length(), in, inLength) != OPENSSL_NPN_NEGOTIATED) {
        return SSL_TLSEXT_ERR_NOACK;
    }
    return SSL_TLSEXT_ERR_OK;
}

void IOCPServer::preConnectCallback([[maybe_unused]] int fd, sese::event::EventLoop *eventLoop, Context *ctx) {
    if (ctx) {
        ctx->client = reinterpret_cast<IOCPService *>(eventLoop);
        onPreConnect(ctx);
        postWrite(ctx);
    }
}

#pragma endregion Server
