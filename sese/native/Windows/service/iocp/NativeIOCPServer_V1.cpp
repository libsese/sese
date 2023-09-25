#include <sese/native/Windows/service/iocp/NativeIOCPServer_V1.h>

#include <openssl/ssl.h>

using namespace sese::net;
using namespace sese::iocp;
using namespace sese::_windows::iocp;

NativeContext_V1::NativeContext_V1(OverlappedWrapper *pWrapper) : pWrapper(pWrapper) {
    wsabufWrite.buf = (CHAR *) malloc(IOCP_WSABUF_SIZE);
}

NativeContext_V1::~NativeContext_V1() {
    free(wsabufWrite.buf);
    if (bio) {
        BIO_free((BIO *) bio);
    }
    if (readNode) {
        delete readNode;
    }
}

int64_t NativeContext_V1::read(void *buffer, size_t length) {
    if (ssl) {
        return SSL_read((SSL *) ssl, buffer, (int) length);
    } else {
        return recv.read(buffer, length);
    }
}

int64_t NativeContext_V1::write(const void *buffer, size_t length) {
    if (ssl) {
        return SSL_write((SSL *) ssl, buffer, (int) length);
    } else {
        return send.write(buffer, length);
    }
}

OverlappedWrapper::OverlappedWrapper() : ctx(this) {
}

bool NativeIOCPServer_V1::init() {
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

    acceptThread = std::make_unique<Thread>([this] { acceptThreadProc(); }, "IOCP_0");
    acceptThread->start();

    if (sslCtx) {
        auto serverSSL = (SSL_CTX *) sslCtx->getContext();
        SSL_CTX_set_alpn_select_cb(
                serverSSL,
                (SSL_CTX_alpn_select_cb_func) &alpnCallbackFunction,
                this
        );
    }

    auto method = BIO_meth_new(BIO_get_new_index() | BIO_TYPE_SOURCE_SINK, "bioIOCP");
    BIO_meth_set_ctrl(method, (long (*)(BIO *, int, long, void *)) & NativeIOCPServer_V1::bioCtrl);
    BIO_meth_set_read(method, (int (*)(BIO *, char *, int)) & NativeIOCPServer_V1::bioRead);
    BIO_meth_set_write(method, (int (*)(BIO *, const char *, int)) & NativeIOCPServer_V1::bioWrite);
    this->bioMethod = method;

    return true;
}

void NativeIOCPServer_V1::shutdown() {
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

    for (auto &&item: wrapperSet) {
        Socket::close(item->ctx.fd);
        deleteContextCallback(&item->ctx);
        delete item;
    }
    wrapperSet.clear();

    if (bioMethod) {
        BIO_meth_free((BIO_METHOD *) bioMethod);
        bioMethod = nullptr;
    }
}

void NativeIOCPServer_V1::postRead(NativeIOCPServer_V1::Context *ctx) {
    auto pWrapper = ctx->pWrapper;
    ctx->type = NativeContext_V1::Type::Read;
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
            &(ctx->pWrapper->overlapped),
            nullptr
    );
    auto e = getNetworkError();
    if (nRt == SOCKET_ERROR && e != ERROR_IO_PENDING) {
        wrapperSetMutex.lock();
        Socket::close(ctx->fd);
        ctx->self->getDeleteContextCallback()(ctx);
        wrapperSet.erase(pWrapper);
        if (pWrapper->ctx.timeoutEvent) {
            wheel.cancel(pWrapper->ctx.timeoutEvent);
            pWrapper->ctx.timeoutEvent = nullptr;
        }
        wrapperSetMutex.unlock();
        delete pWrapper;
    }
}

void NativeIOCPServer_V1::postWrite(NativeIOCPServer_V1::Context *ctx) {
    auto pWrapper = ctx->pWrapper;
    auto len = ctx->send.peek(ctx->wsabufWrite.buf, IOCP_WSABUF_SIZE);
    if (len == 0) {
        return;
    }
    ctx->type = NativeContext_V1::Type::Write;
    ctx->wsabufWrite.len = (ULONG) len;
    DWORD nBytes, dwFlags = 0;
    int nRt = WSASend(
            ctx->fd,
            &ctx->wsabufWrite,
            1,
            &nBytes,
            dwFlags,
            &(ctx->pWrapper->overlapped),
            nullptr
    );
    auto e = getNetworkError();
    if (nRt == SOCKET_ERROR && e != ERROR_IO_PENDING) {
        wrapperSetMutex.lock();
        Socket::close(ctx->fd);
        ctx->self->getDeleteContextCallback()(ctx);
        wrapperSet.erase(pWrapper);
        if (pWrapper->ctx.timeoutEvent) {
            wheel.cancel(pWrapper->ctx.timeoutEvent);
            pWrapper->ctx.timeoutEvent = nullptr;
        }
        wrapperSetMutex.unlock();
        delete pWrapper;
    }
}

void NativeIOCPServer_V1::postClose(NativeIOCPServer_V1::Context *ctx) {
    void *lpCompletionKey = nullptr;
    PostQueuedCompletionStatus(iocpFd, 0, (ULONG_PTR) lpCompletionKey, (LPOVERLAPPED) ctx->pWrapper);
}

void NativeIOCPServer_V1::acceptThreadProc() {
    using namespace std::chrono_literals;

    while (!isShutdown) {

        if (listenFd != INVALID_SOCKET) {
            auto clientSocket = accept(listenFd, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) {
                // std::this_thread::sleep_for(500ms);
                wrapperSetMutex.lock();
                wheel.check();
                wrapperSetMutex.unlock();
                continue;
            }

            if (SOCKET_ERROR == Socket::setNonblocking(clientSocket)) {
                Socket::close(clientSocket);
                // std::this_thread::sleep_for(500ms);
                wrapperSetMutex.lock();
                wheel.check();
                wrapperSetMutex.unlock();
                continue;
            }

            auto pWrapper = new OverlappedWrapper;
            pWrapper->ctx.fd = clientSocket;
            pWrapper->ctx.self = this;

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
                            delete pWrapper;
                            wrapperSetMutex.lock();
                            wheel.check();
                            wrapperSetMutex.unlock();
                            return;
                        }
                    } else {
                        pWrapper->ctx.ssl = clientSSL;
                        SSL_set_mode(clientSSL, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
                        const uint8_t *data = nullptr;
                        uint32_t dataLength;
                        SSL_get0_alpn_selected(clientSSL, &data, &dataLength);
                        onAlpnGet(&pWrapper->ctx, data, dataLength);
                        pWrapper->ctx.bio = BIO_new((BIO_METHOD *) bioMethod);
                        BIO_set_data((BIO *) pWrapper->ctx.bio, &pWrapper->ctx);
                        BIO_set_init((BIO *) pWrapper->ctx.bio, 1);
                        BIO_set_shutdown((BIO *) pWrapper->ctx.bio, 0);
                        SSL_set_bio((SSL *) pWrapper->ctx.ssl, (BIO *) pWrapper->ctx.bio, (BIO *) pWrapper->ctx.bio);
                        break;
                    }
                }
            }

            CreateIoCompletionPort((HANDLE) pWrapper->ctx.fd, iocpFd, 0, 0);

            wrapperSetMutex.lock();
            wrapperSet.emplace(pWrapper);
            wheel.check();
            wrapperSetMutex.unlock();

            onAcceptCompleted(&pWrapper->ctx);
        } else {
            std::this_thread::sleep_for(500ms);
            wrapperSetMutex.lock();
            wheel.check();
            wrapperSetMutex.unlock();
        }
    }
}

void NativeIOCPServer_V1::eventThreadProc() {
    OverlappedWrapper *pWrapper{};
    DWORD lpNumberOfBytesTransferred = 0;
    void *lpCompletionKey = nullptr;

    while (!isShutdown) {
        BOOL bRt = GetQueuedCompletionStatus(
                iocpFd,
                &lpNumberOfBytesTransferred,
                (PULONG_PTR) &lpCompletionKey,
                (LPOVERLAPPED *) &pWrapper,
                INFINITE
        );
        if (!bRt) {
            continue;
        } else if (lpNumberOfBytesTransferred == 0) {
            wrapperSetMutex.lock();
            Socket::close(pWrapper->ctx.fd);
            pWrapper->ctx.self->getDeleteContextCallback()(&pWrapper->ctx);
            wrapperSet.erase(pWrapper);
            if (pWrapper->ctx.timeoutEvent) {
                wheel.cancel(pWrapper->ctx.timeoutEvent);
                pWrapper->ctx.timeoutEvent = nullptr;
            }
            wrapperSetMutex.unlock();
            delete pWrapper;
            continue;
        } else if (lpNumberOfBytesTransferred == -1) {
            break;
        }

        if (pWrapper->ctx.type == NativeContext_V1::Type::Read) {
            onPreRead(&pWrapper->ctx);
            pWrapper->ctx.readNode->size = lpNumberOfBytesTransferred;
            pWrapper->ctx.recv.push(pWrapper->ctx.readNode);
            pWrapper->ctx.readNode = nullptr;
            pWrapper->ctx.wsabufRead.buf = nullptr;
            pWrapper->ctx.wsabufRead.len = 0;
            if (lpNumberOfBytesTransferred == IOCP_WSABUF_SIZE) {
                postRead(&pWrapper->ctx);
            } else {
                onReadCompleted(&pWrapper->ctx);
            }
        } else {
            pWrapper->ctx.send.trunc(lpNumberOfBytesTransferred);
            auto len = pWrapper->ctx.send.peek(pWrapper->ctx.wsabufWrite.buf, IOCP_WSABUF_SIZE);
            if (len == 0) {
                onWriteCompleted(&pWrapper->ctx);
            } else {
                pWrapper->ctx.type = NativeContext_V1::Type::Write;
                pWrapper->ctx.wsabufWrite.len = (ULONG) len;
                DWORD nBytes, dwFlags = 0;
                int nRt = WSASend(
                        pWrapper->ctx.fd,
                        &pWrapper->ctx.wsabufWrite,
                        1,
                        &nBytes,
                        dwFlags,
                        &(pWrapper->overlapped),
                        nullptr
                );
                auto e = getNetworkError();
                if (nRt == SOCKET_ERROR && e != ERROR_IO_PENDING) {
                    wrapperSetMutex.lock();
                    Socket::close(pWrapper->ctx.fd);
                    pWrapper->ctx.self->getDeleteContextCallback()(&pWrapper->ctx);
                    wrapperSet.erase(pWrapper);
                    if (pWrapper->ctx.timeoutEvent) {
                        wheel.cancel(pWrapper->ctx.timeoutEvent);
                        pWrapper->ctx.timeoutEvent = nullptr;
                    }
                    wrapperSetMutex.unlock();
                    delete pWrapper;
                }
            }
        }
    }
}

int NativeIOCPServer_V1::onAlpnSelect(const uint8_t **out, uint8_t *outLength, const uint8_t *in, uint32_t inLength) {
    if (SSL_select_next_proto((unsigned char **) out, outLength, (const uint8_t *) servProtos.c_str(), (int) servProtos.length(), in, inLength) != OPENSSL_NPN_NEGOTIATED) {
        return SSL_TLSEXT_ERR_NOACK;
    }
    return SSL_TLSEXT_ERR_OK;
}

int NativeIOCPServer_V1::alpnCallbackFunction([[maybe_unused]] void *ssl, const uint8_t **out, uint8_t *outLength, const uint8_t *in, uint32_t inLength, NativeIOCPServer_V1 *server) {
    return server->onAlpnSelect(out, outLength, in, inLength);
}

long NativeIOCPServer_V1::bioCtrl(void *bio, int cmd, long num, void *ptr) {
    int ret = 0;
    if (cmd == BIO_CTRL_FLUSH) {
        ret = 1;
    }
    return ret;
}

int NativeIOCPServer_V1::bioWrite(void *bio, const char *in, int length) {
    auto ctx = (NativeContext_V1 *) BIO_get_data((BIO *) bio);
    return (int) ctx->send.write(in, length);
}

int NativeIOCPServer_V1::bioRead(void *bio, char *out, int length) {
    auto ctx = (NativeContext_V1 *) BIO_get_data((BIO *) bio);
    return (int) ctx->recv.read(out, length);
}

void NativeIOCPServer_V1::setTimeout(NativeIOCPServer_V1::Context *ctx, int64_t seconds) {
    wrapperSetMutex.lock();
    ctx->timeoutEvent = wheel.delay(
            [this, ctx]() {
                // auto pWrapper = ctx->pWrapper;
                // Socket::close(pWrapper->ctx.fd);
                // pWrapper->ctx.self->getDeleteContextCallback()(&pWrapper->ctx);
                // wrapperSet.erase(pWrapper);
                // delete ctx->pWrapper;
                ctx->timeoutEvent = nullptr;
                this->onTimeout(ctx);
            },
            seconds, false
    );
    wrapperSetMutex.unlock();
}

void NativeIOCPServer_V1::cancelTimeout(NativeIOCPServer_V1::Context *ctx) {
    if (ctx->timeoutEvent) {
        wrapperSetMutex.lock();
        wheel.cancel(ctx->timeoutEvent);
        ctx->timeoutEvent = nullptr;
        wrapperSetMutex.unlock();
    }
}
