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

IOCPService::IOCPService(IOCPServer *master, bool active_release_mode)
    : TimerableService() {
    IOCPService::master = master;
    IOCPService::handleClose = active_release_mode;

    if (master->getServCtx()) {
        const auto SERVER_SSL = static_cast<SSL_CTX *>(master->getServCtx()->getContext());
        SSL_CTX_set_alpn_select_cb(
                SERVER_SSL,
                reinterpret_cast<SSL_CTX_alpn_select_cb_func>(&alpnCallbackFunction),
                this
        );
    }
}

IOCPService::~IOCPService() {
    for (auto &&item: eventSet) {
        const auto CTX = static_cast<Context *>(item->data);
        if (CTX->ssl) {
            // std::printf("ssl free %p\n", ctx->ssl);
            SSL_free(static_cast<SSL *>(CTX->ssl));
            CTX->ssl = nullptr;
        }
        if (CTX->timeoutEvent) {
            cancelTimeoutEvent(CTX->timeoutEvent);
        }
        sese::net::Socket::close(CTX->fd);
        event::EventLoop::freeEvent(item);
        CTX->self->getDeleteContextCallback()(CTX);
        delete CTX;
    }
}

void IOCPService::postRead(Context *ctx) {
    if (ctx->event) {
        ctx->event->events &= ~EVENT_WRITE;
        ctx->event->events |= EVENT_READ;
        IOCPService::setEvent(ctx->event);
    } else {
        ctx->event = createEventEx(static_cast<int>(ctx->fd), EVENT_READ, ctx);
        ctx->event->data = ctx;
    }
}

void IOCPService::postWrite(Context *ctx) {
    if (ctx->event) {
        ctx->event->events &= ~EVENT_READ;
        ctx->event->events |= EVENT_WRITE;
        IOCPService::setEvent(ctx->event);
    } else {
        ctx->event = createEventEx(static_cast<int>(ctx->fd), EVENT_WRITE, ctx);
        ctx->event->data = ctx;
    }
}

void IOCPService::postClose(Context *ctx) {
    if (handleClose) {
        using namespace sese::net;
        Socket::shutdown(ctx->fd, Socket::ShutdownMode::BOTH);
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

void IOCPService::onAlpnGet(Context *ctx, const uint8_t *in, uint32_t in_length) {
    ctx->self->onAlpnGet(ctx, in, in_length);
}

void IOCPService::onAccept(int fd) {
    if (sese::net::Socket::setNonblocking(fd)) {
        sese::net::Socket::close(fd);
    }

    const auto CTX = new Context;
    CTX->self = master;
    CTX->client = this;
    CTX->fd = fd;

    if (master->getServCtx()) {
        const auto SERVER_SSL = static_cast<SSL_CTX *>(master->getServCtx()->getContext());
        SSL *client_ssl = SSL_new(SERVER_SSL);
        SSL_set_fd(client_ssl, fd);
        SSL_set_accept_state(client_ssl);

        while (true) {
            if (const auto RT = SSL_do_handshake(client_ssl); RT <= 0) {
                if (const auto ERR = SSL_get_error(client_ssl, RT); ERR != SSL_ERROR_WANT_READ && ERR != SSL_ERROR_WANT_WRITE) {
                    SSL_free(client_ssl);
                    sese::net::Socket::close(fd);
                    delete CTX;
                    return;
                }
            } else {
                CTX->ssl = client_ssl;
                SSL_set_mode(client_ssl, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
                const uint8_t *data = nullptr;
                uint32_t data_length;
                SSL_get0_alpn_selected(client_ssl, &data, &data_length);
                onAlpnGet(CTX, data, data_length);
                break;
            }
        }
    }

    onAcceptCompleted(CTX);
}

void IOCPService::onRead(sese::event::BaseEvent *event) {
    const auto CTX = static_cast<Context *>(event->data);

    onPreRead(CTX);

    size_t len = 0;
    while (true) {
        char buf[MTU_VALUE];
        if (const auto L = read(static_cast<int>(CTX->fd), buf, MTU_VALUE, CTX->ssl); L <= 0) {
            if (L == 0 && len == 0) {
                releaseContext(CTX);
                break;
            } else {
                onReadCompleted(CTX);
                break;
            }
        } else {
            CTX->recv.write(buf, L);
            len += static_cast<size_t>(L);
        }
    }
}

void IOCPService::onWrite(sese::event::BaseEvent *event) {
    if (const auto CTX = static_cast<Context *>(event->data); CTX->isConn) {
        CTX->isConn = false;
        auto ssl = static_cast<SSL *>(CTX->ssl);
        if (CTX->ssl) {
            SSL_set_connect_state(ssl);
            // GCOVR_EXCL_START
            while (true) {
                if (const auto RT = SSL_do_handshake(ssl); RT <= 0) {
                    // err is SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE
                    if (const auto ERR = SSL_get_error(ssl, RT); ERR != SSL_ERROR_WANT_READ && ERR != SSL_ERROR_WANT_WRITE) {
                        SSL_free(ssl);
                        CTX->ssl = nullptr;
                        ssl = nullptr;
                        break;
                    }
                } else {
                    break;
                }
            }
            // GCOVR_EXCL_STOP
            if (ssl == nullptr) {
                releaseContext(CTX);
                return;
            } else {
                SSL_set_mode(ssl, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
                const uint8_t *data = nullptr;
                uint32_t data_length;
                SSL_get0_alpn_selected(ssl, &data, &data_length);
                onAlpnGet(CTX, data, data_length);
            }
        }
        onConnected(CTX);
    } else {
        while (true) {
            char buf[MTU_VALUE];
            const auto LEN = CTX->send.peek(buf, MTU_VALUE);
            if (LEN == 0) {
                CTX->send.freeCapacity();
                onWriteCompleted(CTX);
                break;
            }
            if (const auto L = write(static_cast<int>(CTX->fd), buf, LEN, CTX->ssl); L <= 0) {
                if (const auto ERR = sese::net::getNetworkError(); ERR == EWOULDBLOCK || ERR == EINTR) {
                    postWrite(CTX);
                    break;
                } else {
                    releaseContext(CTX);
                    break;
                }
            } else {
                CTX->send.trunc(L);
            }
        }
    }
}

void IOCPService::onClose(sese::event::BaseEvent *event) {
    const auto CTX = static_cast<Context *>(event->data);
    releaseContext(CTX);
}

void IOCPService::onTimeout(v2::TimeoutEvent *event) {
    const auto CTX = static_cast<Context *>(event->data);
    IOCPService::onTimeout(CTX);
}

int IOCPService::alpnCallbackFunction([[maybe_unused]] void *ssl, const uint8_t **out, uint8_t *out_length, const uint8_t *in, uint32_t in_length, IOCPService *service) {
    return service->master->onAlpnSelect(out, out_length, in, in_length);
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
        // std::printf("ssl free %p\n", ctx->ssl);
        SSL_free(static_cast<SSL *>(ctx->ssl));
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
        return SSL_read(static_cast<SSL *>(ssl), buffer, static_cast<int>(len));
    } else {
        return sese::net::Socket::read(fd, buffer, len, 0);
    }
}

int64_t IOCPService::write(int fd, const void *buffer, size_t len, void *ssl) {
    if (ssl) {
        return SSL_write(static_cast<SSL *>(ssl), buffer, static_cast<int>(len));
    } else {
        return sese::net::Socket::write(fd, buffer, len, 0);
    }
}

#pragma endregion Service

#pragma region Server

IOCPServer::IOCPServer() {
    this->balanceLoader.setOnDispatchedCallbackFunction(
            [&](int fd, sese::event::EventLoop *event_loop, void *data) {
                this->preConnectCallback(fd, event_loop, static_cast<Context *>(data));
            }
    );
}

bool IOCPServer::init() {
    if (balanceLoader.init<IOCPService>([this]() -> auto {
            return new IOCPService(this, this->activeReleaseMode);
        })) {
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

void IOCPServer::postConnect(const net::IPAddress::Ptr &to, const security::SSLContext::Ptr &cli_ctx, void *data) {
    const auto SOCK = sese::net::Socket::socket(to->getFamily(), SOCK_STREAM, IPPROTO_IP);
    sese::net::Socket::setNonblocking(SOCK);

    if (connect(SOCK, to->getRawAddress(), to->getRawAddressLength())) {
        if (const auto ERR = sese::net::getNetworkError(); ERR != EINPROGRESS) {
            return;
        }
    }

    const auto CTX = new Context;
    CTX->fd = SOCK;
    CTX->self = this;
    CTX->isConn = true;
    CTX->data = data;
    if (cli_ctx) {
        CTX->ssl = SSL_new(static_cast<SSL_CTX *>(cli_ctx->getContext()));
        // printf("ssl new %p\n", ctx->ssl);
        SSL_set_fd(static_cast<SSL *>(CTX->ssl), static_cast<int>(SOCK));
        SSL_set_alpn_protos(static_cast<SSL *>(CTX->ssl), reinterpret_cast<const unsigned char *>(clientProtos.c_str()), static_cast<unsigned>(clientProtos.length()));
    }

    balanceLoader.dispatchSocket(SOCK, CTX);
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

int IOCPServer::onAlpnSelect(const uint8_t **out, uint8_t *out_length, const uint8_t *in, uint32_t in_length) {
    if (SSL_select_next_proto(const_cast<unsigned char **>(out), out_length, reinterpret_cast<const uint8_t *>(servProtos.c_str()), static_cast<int>(servProtos.length()), in, in_length) != OPENSSL_NPN_NEGOTIATED) {
        return SSL_TLSEXT_ERR_NOACK;
    }
    return SSL_TLSEXT_ERR_OK;
}

void IOCPServer::preConnectCallback([[maybe_unused]] int fd, sese::event::EventLoop *event_loop, Context *ctx) {
    if (ctx) {
        ctx->client = reinterpret_cast<IOCPService *>(event_loop);
        onPreConnect(ctx);
        postWrite(ctx);
    }
}

#pragma endregion Server
