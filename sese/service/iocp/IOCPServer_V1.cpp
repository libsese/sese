#include <sese/service/iocp/IOCPServer_V1.h>

#include <openssl/ssl.h>

using namespace sese::iocp;
using namespace sese::service;

#pragma region Context

int64_t Context_V1::read(void *buffer, size_t length) {
    return recv.read(buffer, length);
}

int64_t Context_V1::write(const void *buffer, size_t length) {
    return send.write(buffer, length);
}

int64_t Context_V1::peek(void *buffer, size_t length) {
    return recv.peek(buffer, length);
}

int64_t Context_V1::trunc(size_t length) {
    return recv.trunc(length);
}

#pragma endregion Context

#pragma region Service

IOCPService_V1::IOCPService_V1(IOCPServer_V1 *master)
    : TimerableService_V2() {
    IOCPService_V1::master = master;

    if (master->getServCtx()) {
        auto serverSSL = (SSL_CTX *) master->getServCtx()->getContext();
        SSL_CTX_set_alpn_select_cb(
                serverSSL,
                (SSL_CTX_alpn_select_cb_func) &alpnCallbackFunction,
                this
        );
    }
}

IOCPService_V1::~IOCPService_V1() {
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

void IOCPService_V1::postRead(IOCPService_V1::Context *ctx) {
    if (ctx->event) {
        ctx->event->events &= ~EVENT_WRITE;
        ctx->event->events |= EVENT_READ;
        IOCPService_V1::setEvent(ctx->event);
    } else {
        ctx->event = createEventEx((int) ctx->fd, EVENT_READ, ctx);
        ctx->event->data = ctx;
    }
}

void IOCPService_V1::postWrite(IOCPService_V1::Context *ctx) {
    if (ctx->event) {
        ctx->event->events &= ~EVENT_READ;
        ctx->event->events |= EVENT_WRITE;
        IOCPService_V1::setEvent(ctx->event);
    } else {
        ctx->event = createEventEx((int) ctx->fd, EVENT_WRITE, ctx);
        ctx->event->data = ctx;
    }
}

void IOCPService_V1::postClose(IOCPService_V1::Context *ctx) {
    // ctx->self->getDeleteContextCallback()(ctx);
    // if (ctx->ssl) {
    //     SSL_free((SSL *) ctx->ssl);
    // }
    // if (ctx->timeoutEvent) {
    //     cancelTimeoutEvent(ctx->timeoutEvent);
    // }
    // sese::net::Socket::close(ctx->fd);
    // ctx->client->freeEventEx(ctx->event);
    // delete ctx;
    using namespace sese::net;
    Socket::shutdown(ctx->fd, Socket::ShutdownMode::Both);
}

void IOCPService_V1::onAcceptCompleted(IOCPService_V1::Context *ctx) {
    ctx->self->onAcceptCompleted(ctx);
}

void IOCPService_V1::onPreRead(IOCPService_V1::Context *ctx) {
    ctx->self->onPreRead(ctx);
}

void IOCPService_V1::onReadCompleted(IOCPService_V1::Context *ctx) {
    ctx->self->onReadCompleted(ctx);
}

void IOCPService_V1::onWriteCompleted(IOCPService_V1::Context *ctx) {
    ctx->self->onWriteCompleted(ctx);
}

void IOCPService_V1::onTimeout(IOCPService_V1::Context *ctx) {
    ctx->timeoutEvent = nullptr;
    ctx->self->onTimeout(ctx);
}

void IOCPService_V1::onConnected(IOCPService_V1::Context *ctx) {
    ctx->self->onConnected(ctx);
}

void IOCPService_V1::onAlpnGet(IOCPService_V1::Context *ctx, const uint8_t *in, uint32_t inLength) {
    ctx->self->onAlpnGet(ctx, in, inLength);
}

void IOCPService_V1::onAccept(int fd) {
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

void IOCPService_V1::onRead(sese::event::BaseEvent *event) {
    auto ctx = (Context *) event->data;

    onPreRead(ctx);

    char buf[MTU_VALUE];
    while (true) {
        auto l = read((int) ctx->fd, buf, MTU_VALUE, ctx->ssl);
        if (l <= 0) {
            auto err = sese::net::getNetworkError();
            if (err == ENOTCONN) {
                ctx->self->getDeleteContextCallback()(ctx);
                if (ctx->ssl) {
                    SSL_free((SSL *) ctx->ssl);
                }
                sese::net::Socket::close(ctx->fd);
                freeEventEx(ctx->event);
                break;
            } else {
                onReadCompleted(ctx);
                break;
            }
        } else {
            ctx->recv.write(buf, l);
        }
    }
}

void IOCPService_V1::onWrite(sese::event::BaseEvent *event) {
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
                        // Socket::close();
                        // return -1;
                        ssl = nullptr;
                        break;
                    }
                } else {
                    break;
                }
            }
            // GCOVR_EXCL_STOP
            if (ssl == nullptr) {
                ctx->self->getDeleteContextCallback()(ctx);
                if (ctx->ssl) {
                    SSL_free((SSL *) ctx->ssl);
                }
                if (ctx->timeoutEvent) {
                    cancelTimeoutEvent(ctx->timeoutEvent);
                    ctx->timeoutEvent = nullptr;
                }
                sese::net::Socket::close(ctx->fd);
                freeEventEx(event);
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
                    ctx->self->getDeleteContextCallback()(ctx);
                    if (ctx->ssl) {
                        SSL_free((SSL *) ctx->ssl);
                    }
                    sese::net::Socket::close(ctx->fd);
                    freeEventEx(ctx->event);
                    break;
                }
            } else {
                ctx->send.trunc(l);
            }
        }
    }
}

void IOCPService_V1::onClose(sese::event::BaseEvent *event) {
    auto ctx = (Context *) event->data;
    ctx->self->getDeleteContextCallback()(ctx);
    if (ctx->ssl) {
        SSL_free((SSL *) ctx->ssl);
    }
    if (ctx->timeoutEvent) {
        cancelTimeoutEvent(ctx->timeoutEvent);
    }
    sese::net::Socket::close(ctx->fd);
    ctx->client->freeEventEx(ctx->event);
    delete ctx;
}

void IOCPService_V1::onTimeout(TimeoutEvent_V2 *event) {
    auto ctx = (Context *) event->data;
    IOCPService_V1::onTimeout(ctx);
}

int IOCPService_V1::alpnCallbackFunction([[maybe_unused]] void *ssl, const uint8_t **out, uint8_t *outLength, const uint8_t *in, uint32_t inLength, IOCPService_V1 *service) {
    return service->master->onAlpnSelect(out, outLength, in, inLength);
}

sese::event::BaseEvent *IOCPService_V1::createEventEx(int fd, unsigned int events, void *data) {
    auto event = createEvent(fd, events, data);
    eventSet.emplace(event);
    return event;
}

void IOCPService_V1::freeEventEx(sese::event::BaseEvent *event) {
    eventSet.erase(event);
    freeEvent(event);
}

int64_t IOCPService_V1::read(int fd, void *buffer, size_t len, void *ssl) {
    if (ssl) {
        return SSL_read((SSL *) ssl, buffer, (int) len);
    } else {
        return sese::net::Socket::read(fd, buffer, len, 0);
    }
}

int64_t IOCPService_V1::write(int fd, const void *buffer, size_t len, void *ssl) {
    if (ssl) {
        return SSL_write((SSL *) ssl, buffer, (int) len);
    } else {
        return sese::net::Socket::write(fd, buffer, len, 0);
    }
}

#pragma endregion Service

#pragma region Server

IOCPServer_V1::IOCPServer_V1() {
    this->balanceLoader.setOnDispatchedCallbackFunction(
            [&](int fd, sese::event::EventLoop *eventLoop, void *data) {
                this->preConnectCallback(fd, eventLoop, (Context *) data);
            }
    );
}

bool IOCPServer_V1::init() {
    auto rt = balanceLoader.init<IOCPService_V1>([this]() -> auto {
        return new IOCPService_V1(this);
    });
    if (rt) {
        balanceLoader.start();
        return true;
    }
    return false;
}

void IOCPServer_V1::shutdown() {
    balanceLoader.stop();
}

void IOCPServer_V1::postRead(IOCPServer_V1::Context *ctx) {
    ctx->client->postRead(ctx);
}

void IOCPServer_V1::postWrite(IOCPService_V1::Context *ctx) {
    ctx->client->postWrite(ctx);
}

void IOCPServer_V1::postClose(IOCPServer_V1::Context *ctx) {
    ctx->client->postClose(ctx);
}

void IOCPServer_V1::postConnect(socket_t sock, const net::IPAddress::Ptr &to, const security::SSLContext::Ptr &cliCtx) {
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
    if (cliCtx) {
        ctx->ssl = SSL_new((SSL_CTX *) cliCtx->getContext());
        SSL_set_fd((SSL *) ctx->ssl, (int) sock);
    }

    balanceLoader.dispatchSocket(sock, ctx);
}

void IOCPServer_V1::setTimeout(IOCPServer_V1::Context *ctx, int64_t seconds) {
    if (ctx->timeoutEvent) {
        ctx->client->cancelTimeoutEvent(ctx->timeoutEvent);
        ctx->timeoutEvent = nullptr;
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
    if (SSL_select_next_proto((unsigned char **) out, outLength, (const uint8_t *) servProtos.c_str(), (int) servProtos.length(), in, inLength) != OPENSSL_NPN_NEGOTIATED) {
        return SSL_TLSEXT_ERR_NOACK;
    }
    return SSL_TLSEXT_ERR_OK;
}

void IOCPServer_V1::preConnectCallback([[maybe_unused]] int fd, sese::event::EventLoop *eventLoop, sese::iocp::IOCPServer_V1::Context *ctx) {
    if (ctx) {
        ctx->client = reinterpret_cast<IOCPService_V1 *>(eventLoop);
        onPreConnect(ctx);
        postWrite(ctx);
    }
}

#pragma endregion Service
