// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <sese/net/Socket.h>
#include <sese/service/TcpTransporter.h>

#include <openssl/ssl.h>

void sese::service::TcpTransporterConfig::freeConnection(sese::service::TcpConnection *conn) {
    delete conn;
}

sese::service::TcpTransporter::TcpTransporter(sese::service::TcpTransporterConfig *transporter_config) noexcept {
    this->config = transporter_config;

    if (this->config->servCtx) {
        // ALPN
        SSL_CTX_set_alpn_select_cb(
                static_cast<SSL_CTX *>(config->servCtx->getContext()),
                reinterpret_cast<SSL_CTX_alpn_select_cb_func>(&alpnCallbackFunction),
                this
        );
    }
}

sese::service::TcpTransporter::~TcpTransporter() {
    for (auto &item: eventMap) {
        auto event = item.second;
        auto conn = static_cast<TcpConnection *>(event->data);
        if (conn->timeoutEvent) {
            this->freeTimeoutEvent(conn->timeoutEvent);
        }
        if (config->servCtx) {
            SSL_free(static_cast<SSL *>(conn->ssl));
        }
        sese::net::Socket::close(event->fd);
        // delete conn;
        config->freeConnection(conn);
        this->freeEvent(event);
    }
    eventMap.clear();
}

void sese::service::TcpTransporter::onAccept(int fd) {
    SSL *client_ssl;
    // GCOVR_EXCL_START
    if (sese::net::Socket::setNonblocking(fd)) {
        sese::net::Socket::close(fd);
    }

    // auto conn = new TcpConnection;
    auto conn = config->createConnection();

    // GCOVR_EXCL_STOP
    if (config->servCtx) {
        client_ssl = SSL_new(static_cast<SSL_CTX *>(config->servCtx->getContext()));
        SSL_set_fd(client_ssl, (int) fd);
        SSL_set_accept_state(client_ssl);

        // GCOVR_EXCL_START
        while (true) {
            auto rt = SSL_do_handshake(client_ssl);
            if (rt <= 0) {
                auto err = SSL_get_error(client_ssl, rt);
                if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                    SSL_free(client_ssl);
                    sese::net::Socket::close(fd);
                    // delete conn;
                    config->freeConnection(conn);
                    return;
                }
            } else {
                conn->ssl = client_ssl;
                // This option allows OpenSSL to use different buffer parameters when trying to retry SSL_write
                // https://stackoverflow.com/questions/2997218/why-am-i-getting-error1409f07fssl-routinesssl3-write-pending-bad-write-retr
                SSL_set_mode(client_ssl, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
                // ALPN Callback
                const uint8_t *data = nullptr;
                uint32_t data_length;
                SSL_get0_alpn_selected(client_ssl, &data, &data_length);
                onProcAlpnGet(conn, data, data_length);
                break;
            }
        }
        // GCOVR_EXCL_STOP
    }

    if (config->keepalive) {
        conn->timeoutEvent = createTimeoutEvent(fd, conn, config->keepalive);
    }
    conn->event = createEventEx(fd, EVENT_READ, conn);
}

void sese::service::TcpTransporter::onRead(sese::event::BaseEvent *event) {
    auto conn = static_cast<TcpConnection *>(event->data);

    if (config->keepalive && conn->timeoutEvent) {
        cancelTimeoutEvent(conn->timeoutEvent);
    }

    char buf[MTU_VALUE];
    while (true) {
        auto l = read(event->fd, buf, MTU_VALUE, conn->ssl);
        if (l <= 0) {
            auto err = sese::net::getNetworkError();
            if (err == ENOTCONN) {
                if (conn->timeoutEvent) {
                    TimerableService::freeTimeoutEvent(conn->timeoutEvent);
                }
                onProcClose(conn);
                if (config->servCtx) {
                    SSL_free(static_cast<SSL *>(conn->ssl));
                }
                sese::net::Socket::close(event->fd);
                // delete conn;
                config->freeConnection(conn);
                this->freeEventEx(event);
                break;
            } else {
                onProcHandle(conn);
                break;
            }
        } else {
            conn->buffer2read.write(buf, l);
        }
    }
}

void sese::service::TcpTransporter::onWrite(sese::event::BaseEvent *event) {
    auto conn = static_cast<TcpConnection *>(event->data);
    char buf[MTU_VALUE];
    while (true) {
        auto len = conn->buffer2write.peek(buf, MTU_VALUE);
        if (len == 0) {
            conn->buffer2write.freeCapacity();
            break;
        }
        auto l = write(event->fd, buf, len, conn->ssl);
        if (l <= 0) {
            auto err = sese::net::getNetworkError();
            if (err == EWOULDBLOCK || err == EINTR) {
                conn->event->events &= ~EVENT_READ;
                conn->event->events |= EVENT_WRITE;
                this->setEvent(event);
                break;
            } else {
                if (conn->timeoutEvent) {
                    TimerableService::freeTimeoutEvent(conn->timeoutEvent);
                }
                onProcClose(conn);
                if (config->servCtx) {
                    SSL_free(static_cast<SSL *>(conn->ssl));
                }
                sese::net::Socket::close(event->fd);
                // delete conn;
                config->freeConnection(conn);
                this->freeEventEx(event);
                break;
            }
        } else {
            conn->buffer2write.trunc(l);
        }
    }

    conn->event->events |= EVENT_READ;
    conn->event->events &= ~EVENT_WRITE;
    this->setEvent(event);
}

void sese::service::TcpTransporter::onClose(sese::event::BaseEvent *event) {
    auto conn = static_cast<TcpConnection *>(event->data);
    /// \brief If a connection is processed asynchronously,
    /// the peer closure event should not release resources for the connection
    /// \see tcp_connection_delay_close_by_async
    if (!conn->isAsync) {
        if (conn->timeoutEvent) {
            TimerableService::freeTimeoutEvent(conn->timeoutEvent);
        }
        onProcClose(conn);
        if (config->servCtx) {
            SSL_free(static_cast<SSL *>(conn->ssl));
        }
        sese::net::Socket::close(event->fd);
        // delete conn;
        config->freeConnection(conn);
        freeEventEx(event);
    }
}

sese::event::BaseEvent *sese::service::TcpTransporter::createEventEx(int fd, unsigned int events, sese::service::TcpConnection *conn) noexcept {
    auto event = createEvent(fd, events, conn);
    eventMap[fd] = event;
    return event;
}

void sese::service::TcpTransporter::freeEventEx(event::BaseEvent *event) noexcept {
    eventMap.erase(event->fd);
    freeEvent(event);
}

int64_t sese::service::TcpTransporter::read(int fd, void *buffer, size_t len, void *ssl) noexcept {
    if (ssl) {
        return SSL_read(static_cast<SSL *>(ssl), buffer, static_cast<int>(len));
    } else {
        return sese::net::Socket::read(fd, buffer, len, 0);
    }
}

int64_t sese::service::TcpTransporter::write(int fd, const void *buffer, size_t len, void *ssl) noexcept {
    if (ssl) {
        return SSL_write(static_cast<SSL *>(ssl), buffer, static_cast<int>(len));
    } else {
        return sese::net::Socket::write(fd, buffer, len, 0);
    }
}

void sese::service::TcpTransporter::onTimeout(sese::service::v1::TimeoutEvent *timeout_event) {
    auto conn = static_cast<TcpConnection *>(timeout_event->data);
    onProcClose(conn);
    if (config->servCtx) {
        SSL_free(static_cast<SSL *>(conn->ssl));
    }
    sese::net::Socket::close(conn->event->fd);
    this->freeEventEx(conn->event);
    // delete conn;
    config->freeConnection(conn);
}

void sese::service::TcpTransporter::postRead(TcpConnection *conn) {
    conn->event->events &= ~EVENT_WRITE;
    conn->event->events |= EVENT_READ;
    setEvent(conn->event);
}

void sese::service::TcpTransporter::postWrite(TcpConnection *conn) {
    conn->event->events |= EVENT_WRITE;
    conn->event->events &= ~EVENT_READ;
    onWrite(conn->event);
}

int sese::service::TcpTransporter::alpnCallbackFunction([[maybe_unused]] void *ssl, const uint8_t **out, uint8_t *out_length, const uint8_t *in, uint32_t in_length, sese::service::TcpTransporter *transporter) {
    return transporter->onProcAlpnSelect(out, out_length, in, in_length);
}