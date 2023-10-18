#include <sese/native/Windows/service/iocp/NativeIOCPServer_V1.h>
#include <sese/record/Marco.h>

#include <openssl/ssl.h>

#include <mswsock.h>

using namespace sese::net;
using namespace sese::iocp;
using namespace sese::_windows::iocp::v1;

NativeContext::NativeContext(OverlappedWrapper *pWrapper) : pWrapper(pWrapper) {
    wsabufWrite.buf = (CHAR *) malloc(IOCP_WSABUF_SIZE);
}

NativeContext::~NativeContext() {
    free(wsabufWrite.buf);
    if (readNode) { // NOLINT
        delete readNode;
    }
}

int64_t NativeContext::read(void *buffer, size_t length) {
    if (ssl) {
        return SSL_read((SSL *) ssl, buffer, (int) length);
    } else {
        return recv.read(buffer, length);
    }
}

int64_t NativeContext::write(const void *buffer, size_t length) {
    if (ssl) {
        return SSL_write((SSL *) ssl, buffer, (int) length);
    } else {
        return send.write(buffer, length);
    }
}

int64_t NativeContext::peek(void *buffer, size_t length) {
    if (ssl) {
        return SSL_peek((SSL *) ssl, buffer, (int) length);
    } else {
        return recv.peek(buffer, length);
    }
}

int64_t NativeContext::trunc(size_t length) {
    if (ssl) {
        char buffer[1024]{};
        int64_t real = 0;
        while (true) {
            auto need = std::min<int>(static_cast<int>(length - real), sizeof(buffer));
            int l = SSL_read((SSL *) ssl, buffer, need);
            if (l > 0) {
                real += l;
            } else {
                break;
            }
            if (real == length) {
                break;
            }
        }
        return real;
    } else {
        return recv.trunc(length);
    }
}

OverlappedWrapper::OverlappedWrapper() : ctx(this) {
}

bool NativeIOCPServer::init() {
    if (!initConnectEx()) {
        return false;
    }

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
    BIO_meth_set_ctrl(method, (long (*)(BIO *, int, long, void *)) & NativeIOCPServer::bioCtrl);
    BIO_meth_set_read(method, (int (*)(BIO *, char *, int)) & NativeIOCPServer::bioRead);
    BIO_meth_set_write(method, (int (*)(BIO *, const char *, int)) & NativeIOCPServer::bioWrite);
    this->bioMethod = method;

    return true;
}

void NativeIOCPServer::shutdown() {
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
        deleteContextCallback(&item->ctx);
        Socket::close(item->ctx.fd);
        delete item;
    }
    wrapperSet.clear();

    if (bioMethod) {
        BIO_meth_free((BIO_METHOD *) bioMethod);
        bioMethod = nullptr;
    }
}

void NativeIOCPServer::postRead(NativeIOCPServer::Context *ctx) {
    ctx->type = NativeContext::Type::Read;
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
        releaseContext(ctx);
    }
}

void NativeIOCPServer::postWrite(NativeIOCPServer::Context *ctx) {
    auto len = ctx->send.peek(ctx->wsabufWrite.buf, IOCP_WSABUF_SIZE);
    if (len == 0) {
        return;
    }
    ctx->type = NativeContext::Type::Write;
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
        releaseContext(ctx);
    }
}

void NativeIOCPServer::postClose(NativeIOCPServer::Context *ctx) {
    if (activeReleaseMode) {
        void *lpCompletionKey = nullptr;
        ctx->type = Context::Type::Close;
        PostQueuedCompletionStatus(iocpFd, 0, (ULONG_PTR) lpCompletionKey, (LPOVERLAPPED) ctx->pWrapper);
    } else {
        releaseContext(ctx);
    }
}

#define ConnectEx ((LPFN_CONNECTEX) connectEx)

void NativeIOCPServer::postConnect(const net::IPAddress::Ptr &to, const security::SSLContext::Ptr &cliCtx, void *data) {
    auto sock = sese::net::Socket::socket(to->getFamily(), SOCK_STREAM, IPPROTO_IP);
    if (to->getFamily() == AF_INET) {
        auto from = sese::net::IPv4Address::any();
        sese::net::Socket::bind(sock, from->getRawAddress(), from->getRawAddressLength());
    } else {
        auto from = sese::net::IPv6Address::any();
        sese::net::Socket::bind(sock, from->getRawAddress(), from->getRawAddressLength());
    }

    sese::net::Socket::setNonblocking(sock);

    auto pWrapper = new OverlappedWrapper();
    pWrapper->ctx.fd = sock;
    pWrapper->ctx.self = this;
    pWrapper->ctx.type = NativeContext::Type::Connect;
    pWrapper->ctx.data = data;
    if (cliCtx) {
        pWrapper->ctx.ssl = SSL_new((SSL_CTX *) cliCtx->getContext());
        SSL_set_fd((SSL *) pWrapper->ctx.ssl, (int) sock);
        SSL_set_alpn_protos((SSL *) pWrapper->ctx.ssl, (const unsigned char *) clientProtos.c_str(), (unsigned) clientProtos.length());
    }
    auto addr = to->getRawAddress();
    auto len = to->getRawAddressLength();

    CreateIoCompletionPort((HANDLE) pWrapper->ctx.fd, iocpFd, 0, 0);
    BOOL nRt = ConnectEx(sock, addr, len, nullptr, 0, nullptr, (LPOVERLAPPED) pWrapper);
    auto e = getNetworkError();
    if (nRt == FALSE && e != ERROR_IO_PENDING) {
        releaseContext(&pWrapper->ctx);
    } else {
        wrapperSetMutex.lock();
        wrapperSet.emplace(pWrapper);
        wrapperSetMutex.unlock();
        onPreConnect(&pWrapper->ctx);
    }
}

#undef ConnectEx

void NativeIOCPServer::acceptThreadProc() {
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
                            Socket::close(clientSocket);
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

void NativeIOCPServer::eventThreadProc() {
    OverlappedWrapper *pWrapper{};
    DWORD lpNumberOfBytesTransferred = 0;
    void *lpCompletionKey = nullptr;

    while (!isShutdown) {
        GetQueuedCompletionStatus(
                iocpFd,
                &lpNumberOfBytesTransferred,
                (PULONG_PTR) &lpCompletionKey,
                (LPOVERLAPPED *) &pWrapper,
                INFINITE
        );
        if (pWrapper == nullptr) {
            continue;
        } else if (lpNumberOfBytesTransferred == 0 && pWrapper->ctx.type != NativeContext::Type::Connect) {
            // 主动释放模式对端关闭
            // 任何模式下的非主动关闭
            if (activeReleaseMode || pWrapper->ctx.type != NativeContext::Type::Close) {
                releaseContext(&pWrapper->ctx);
            }
            continue;
        } else if (lpNumberOfBytesTransferred == -1) {
            break;
        }

        if (pWrapper->ctx.type == NativeContext::Type::Read) {
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
        } else if (pWrapper->ctx.type == NativeContext::Type::Write) {
            pWrapper->ctx.send.trunc(lpNumberOfBytesTransferred);
            auto len = pWrapper->ctx.send.peek(pWrapper->ctx.wsabufWrite.buf, IOCP_WSABUF_SIZE);
            if (len == 0) {
                onWriteCompleted(&pWrapper->ctx);
            } else {
                pWrapper->ctx.type = NativeContext::Type::Write;
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
                    releaseContext(&pWrapper->ctx);
                }
            }
        } else {
            auto connectStatus = GetOverlappedResult((HANDLE) pWrapper->ctx.fd, (LPOVERLAPPED) pWrapper, &lpNumberOfBytesTransferred, TRUE);
            if (connectStatus == FALSE) {
                releaseContext(&pWrapper->ctx);
                continue;
            }

            auto ssl = (SSL *) pWrapper->ctx.ssl;
            if (ssl) {
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
                    releaseContext(&pWrapper->ctx);
                    continue;
                } else {
                    SSL_set_mode(ssl, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
                    const uint8_t *data = nullptr;
                    uint32_t dataLength;
                    SSL_get0_alpn_selected(ssl, &data, &dataLength);
                    onAlpnGet(&pWrapper->ctx, data, dataLength);
                    pWrapper->ctx.bio = BIO_new((BIO_METHOD *) bioMethod);
                    BIO_set_data((BIO *) pWrapper->ctx.bio, &pWrapper->ctx);
                    BIO_set_init((BIO *) pWrapper->ctx.bio, 1);
                    BIO_set_shutdown((BIO *) pWrapper->ctx.bio, 0);
                    SSL_set_bio((SSL *) pWrapper->ctx.ssl, (BIO *) pWrapper->ctx.bio, (BIO *) pWrapper->ctx.bio);
                }
            }
            pWrapper->ctx.type = Context::Type::Ready;
            pWrapper->ctx.self->onConnected(&pWrapper->ctx);
        }
    }
}

int NativeIOCPServer::onAlpnSelect(const uint8_t **out, uint8_t *outLength, const uint8_t *in, uint32_t inLength) {
    if (SSL_select_next_proto((unsigned char **) out, outLength, (const uint8_t *) servProtos.c_str(), (int) servProtos.length(), in, inLength) != OPENSSL_NPN_NEGOTIATED) {
        return SSL_TLSEXT_ERR_NOACK;
    }
    return SSL_TLSEXT_ERR_OK;
}

int NativeIOCPServer::alpnCallbackFunction([[maybe_unused]] void *ssl, const uint8_t **out, uint8_t *outLength, const uint8_t *in, uint32_t inLength, NativeIOCPServer *server) {
    return server->onAlpnSelect(out, outLength, in, inLength);
}

bool NativeIOCPServer::initConnectEx() {
    auto sock = ::socket(AF_INET, SOCK_STREAM, 0);
    DWORD dwBytes;
    GUID guid = WSAID_CONNECTEX;
    auto rc = WSAIoctl(
            sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
            &guid, sizeof(guid),
            &connectEx, sizeof(connectEx),
            &dwBytes, nullptr, nullptr
    );
    Socket::close(sock);
    return rc == 0;
}

long NativeIOCPServer::bioCtrl([[maybe_unused]] void *bio, int cmd, [[maybe_unused]] long num, [[maybe_unused]] void *ptr) {
    int ret = 0;
    if (cmd == BIO_CTRL_FLUSH) {
        ret = 1;
    }
    return ret;
}

int NativeIOCPServer::bioWrite(void *bio, const char *in, int length) {
    auto ctx = (NativeContext *) BIO_get_data((BIO *) bio);
    return (int) ctx->send.write(in, length);
}

int NativeIOCPServer::bioRead(void *bio, char *out, int length) {
    auto ctx = (NativeContext *) BIO_get_data((BIO *) bio);
    return (int) ctx->recv.read(out, length);
}

void NativeIOCPServer::setTimeout(NativeIOCPServer::Context *ctx, int64_t seconds) {
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

void NativeIOCPServer::cancelTimeout(NativeIOCPServer::Context *ctx) {
    if (ctx->timeoutEvent) {
        wrapperSetMutex.lock();
        wheel.cancel(ctx->timeoutEvent);
        ctx->timeoutEvent = nullptr;
        wrapperSetMutex.unlock();
    }
}

void NativeIOCPServer::releaseContext(Context *ctx) {
    wrapperSetMutex.lock();
    auto pWrapper = ctx->pWrapper;
    wrapperSet.erase(pWrapper);
    if (pWrapper->ctx.timeoutEvent) {
        wheel.cancel(pWrapper->ctx.timeoutEvent);
        pWrapper->ctx.timeoutEvent = nullptr;
    }
    wrapperSetMutex.unlock();
    Socket::close(pWrapper->ctx.fd);
    if (pWrapper->ctx.ssl) {
        SSL_free((SSL *) pWrapper->ctx.ssl);
        pWrapper->ctx.ssl = nullptr;
    }
    pWrapper->ctx.self->getDeleteContextCallback()(&pWrapper->ctx);
    delete pWrapper;
}
