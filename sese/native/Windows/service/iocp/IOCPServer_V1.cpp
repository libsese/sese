#include <sese/native/Windows/service/iocp/IOCPServer_V1.h>

#include <openssl/ssl.h>

using namespace sese::net;
using namespace sese::iocp;
using namespace sese::_windows::iocp;

Context_V1::Context_V1() {
    wsabufWrite.buf = (CHAR *) malloc(IOCP_WSABUF_SIZE);
}

Context_V1::~Context_V1() {
    free(wsabufWrite.buf);
}

bool IOCPServer_V1::init() {
    if (address) {
        listenFd = WSASocketW(
                address->getFamily(),
                SOCK_STREAM,
                0,
                nullptr,
                0,
                WSA_FLAG_OVERLAPPED
        );
        if (listenFd == INVALID_SOCKET) {
            return false;
        }
        if (SOCKET_ERROR == Socket::setNonblocking(listenFd)) {
            return false;
        }
        if (SOCKET_ERROR == Socket::bind(listenFd, address->getRawAddress(), address->getRawAddressLength())) {
            return false;
        }
        if (SOCKET_ERROR == Socket::listen(listenFd, SOMAXCONN)) {
            return false;
        }
    }

    iocpFd = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, (DWORD) threads);
    if (iocpFd == INVALID_HANDLE_VALUE) {
        if (address) {
            Socket::close(listenFd);
            listenFd = INVALID_SOCKET;
        }
        return false;
    }

    for (int i = 0; i < threads; ++i) {
        auto th = std::make_unique<Thread>([this] { eventThreadProc(); }, "IOCP_" + std::to_string(i + 1));
        th->start();
        eventThreadGroup.emplace_back(std::move(th));
    }
    if (address) {
        acceptThread = std::make_unique<Thread>([this] { acceptThreadProc(); }, "IOCP_0");
        acceptThread->start();
    }

    if (sslCtx) {
        auto serverSSL = (SSL_CTX *) sslCtx->getContext();
        SSL_CTX_set_alpn_select_cb(
                serverSSL,
                (SSL_CTX_alpn_select_cb_func) &alpnCallbackFunction,
                this
        );
    }

    return true;
}

void IOCPServer_V1::shutdown() {
    void *lpCompletionKey = nullptr;
    isShutdown = true;
    for (int i = 0; i < threads; ++i) {
        PostQueuedCompletionStatus(iocpFd, -1, (ULONG_PTR) lpCompletionKey, nullptr);
    }
    for (auto &&thread: eventThreadGroup) {
        thread->join();
    }
    eventThreadGroup.clear();
    acceptThread->join();
    acceptThread = nullptr;
}

void IOCPServer_V1::postRead(IOCPServer_V1::Context *ctx) {
    ctx->type = Context_V1::Type::Read;
    ctx->readNode = new IOBufNode(IOCP_WSABUF_SIZE);
    ctx->wsabufRead.buf = (CHAR *) ctx->readNode->buffer;
    ctx->wsabufRead.len = (ULONG) ctx->readNode->capacity;
    DWORD nBytes, dwFlags = 0;
    int nRt = WSARecv(
            ctx->fd,
            &ctx->wsabufRead,
            1,
            &nBytes,
            &dwFlags,
            &(ctx->overlapped),
            nullptr
    );
    auto e = getNetworkError();
    if (nRt == SOCKET_ERROR && e != ERROR_IO_PENDING) {
        Socket::close(ctx->fd);
        ctx->self->getDeleteContextCallback()(ctx);
        puts(getNetworkErrorString(e).c_str());
        delete ctx;
    }
}

void IOCPServer_V1::postWrite(IOCPServer_V1::Context *ctx) {
    auto len = ctx->send.peek(ctx->wsabufWrite.buf, IOCP_WSABUF_SIZE);
    if (len == 0) {
        return;
    }
    ctx->type = Context_V1::Type::Write;
    ctx->wsabufWrite.len = (ULONG) len;
    DWORD nBytes, dwFlags = 0;
    int nRt = WSASend(
            ctx->fd,
            &ctx->wsabufWrite,
            1,
            &nBytes,
            dwFlags,
            &(ctx->overlapped),
            nullptr
    );
    auto e = getNetworkError();
    if (nRt == SOCKET_ERROR && e != ERROR_IO_PENDING) {
        Socket::close(ctx->fd);
        ctx->self->getDeleteContextCallback()(ctx);
        delete ctx;
    }
}

void IOCPServer_V1::acceptThreadProc() {
    while (!isShutdown) {
        auto clientSocket = accept(listenFd, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            continue;
        }

        if (SOCKET_ERROR == Socket::setNonblocking(clientSocket)) {
            Socket::close(clientSocket);
            continue;
        }

        auto ctx = new Context;
        ctx->fd = clientSocket;
        ctx->self = this;

        if (sslCtx) {
            auto serverSSL = (SSL_CTX *) sslCtx->getContext();
            auto clientSSL = SSL_new(serverSSL);
            SSL_set_fd(clientSSL, (int) clientSocket);
            SSL_set_accept_state(clientSSL);

            while (true) {
                auto rt = SSL_do_handshake(clientSSL);
                if (rt <= 0) {
                    auto err = SSL_get_error(clientSSL, rt);
                    if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                        SSL_free(clientSSL);
                        sese::net::Socket::close(clientSocket);
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

        CreateIoCompletionPort((HANDLE)ctx->fd, iocpFd, 0, 0);

        onAcceptCompleted(ctx);
    }
}

void IOCPServer_V1::eventThreadProc() {
    Context *ctx = nullptr;
    DWORD lpNumberOfBytesTransferred = 0;
    void *lpCompletionKey = nullptr;

    while (!isShutdown) {
        BOOL bRt = GetQueuedCompletionStatus(
                iocpFd,
                &lpNumberOfBytesTransferred,
                (PULONG_PTR) &lpCompletionKey,
                (LPOVERLAPPED *) &ctx,
                INFINITE
        );
        if (!bRt || lpNumberOfBytesTransferred == 0) {
            continue;
        } else if (lpNumberOfBytesTransferred == -1) {
            break;
        }

        if (ctx->type == Context_V1::Type::Read) {
            onPreRead(ctx);
            ctx->readNode->size = lpNumberOfBytesTransferred;
            ctx->recv.push(ctx->readNode);
            ctx->readNode = nullptr;
            ctx->wsabufRead.buf = nullptr;
            ctx->wsabufRead.len = 0;
            if (lpNumberOfBytesTransferred == IOCP_WSABUF_SIZE) {
                postRead(ctx);
            } else {
                onReadCompleted(ctx);
            }
        } else {
            ctx->send.trunc(lpNumberOfBytesTransferred);
            auto len = ctx->send.peek(ctx->wsabufWrite.buf, IOCP_WSABUF_SIZE);
            if (len == 0) {
                onWriteCompleted(ctx);
            } else {
                ctx->type = Context_V1::Type::Write;
                ctx->wsabufWrite.len = (ULONG) len;
                DWORD nBytes, dwFlags = 0;
                int nRt = WSASend(
                        ctx->fd,
                        &ctx->wsabufWrite,
                        1,
                        &nBytes,
                        dwFlags,
                        &(ctx->overlapped),
                        nullptr
                );
                auto e = getNetworkError();
                if (nRt == SOCKET_ERROR && e != ERROR_IO_PENDING) {
                    Socket::close(ctx->fd);
                    ctx->self->getDeleteContextCallback()(ctx);
                    delete ctx;
                }
            }
        }
    }
}

int IOCPServer_V1::onAlpnSelect(const uint8_t **out, uint8_t *outLength, const uint8_t *in, uint32_t inLength) {
    if (SSL_select_next_proto((unsigned char **) out, outLength, (const uint8_t *) servProtos.c_str(), (int) servProtos.length(), in, inLength) != OPENSSL_NPN_NEGOTIATED) {
        return SSL_TLSEXT_ERR_NOACK;
    }
    return SSL_TLSEXT_ERR_OK;
}

int IOCPServer_V1::alpnCallbackFunction([[maybe_unused]] void *ssl, const uint8_t **out, uint8_t *outLength, const uint8_t *in, uint32_t inLength, IOCPServer_V1 *server) {
    return server->onAlpnSelect(out, outLength, in, inLength);
}

void IOCPServer_V1::setTimeout(IOCPServer_V1::Context *ctx, int64_t seconds) {}

void IOCPServer_V1::cancelTimeout(IOCPServer_V1::Context *ctx) {}
