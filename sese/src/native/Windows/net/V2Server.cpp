#include "sese/net/V2Server.h"
#include "openssl/ssl.h"

static long bio_iocp_ctrl([[maybe_unused]] BIO *bio, int cmd, [[maybe_unused]] long num, [[maybe_unused]] void *ptr) {
    int ret = 0;
    if (cmd == BIO_CTRL_FLUSH) {
        ret = 1;
    }
    return ret;
}

static int bio_iocp_write(BIO *bio, const char *in, int length) {
    auto ctx = (sese::net::v2::IOContext *) BIO_get_data(bio);
    return ::send(ctx->socket, in, length, 0);
}

static int bio_iocp_read(BIO *bio, char *out, int length) {
    auto ctx = (sese::net::v2::IOContext *) BIO_get_data(bio);
    if (ctx->nRead < ctx->nBytes) {
        if ((int) (ctx->nBytes - ctx->nRead) > length) {
            memcpy(out, ctx->buffer + ctx->nRead, length);
            ctx->nRead += (DWORD) length;
            BIO_clear_retry_flags(bio);
            return (int) length;
        } else {
            auto rt = ctx->nBytes - ctx->nRead;
            memcpy(out, ctx->buffer + ctx->nRead, rt);
            ctx->nRead = ctx->nBytes;
            BIO_clear_retry_flags(bio);
            return (int) rt;
        }
    } else {
        auto rt = ::recv(ctx->socket, out, length, 0);
        BIO_clear_retry_flags(bio);
        return rt;
    }
}

int64_t sese::net::v2::IOContext::readWithoutSSL(void *buf, size_t length) noexcept {
    // 缓冲区内有未读字节
    if (this->nRead < this->nBytes) {
        // 缓冲区够用
        if (this->nBytes - this->nRead > length) {
            memcpy(buf, this->buffer + this->nRead, length);
            this->nRead += (DWORD) length;
            return (int64_t) length;
        }
        // 缓冲区不够用
        // 直接返回当前剩余部分
        else {
            memcpy(buf, this->buffer + this->nRead, this->nBytes - this->nRead);
            this->nRead = this->nBytes;
            return this->nBytes - this->nRead;
        }
    }
    // 缓冲区已空
    else {
        return ::recv(socket, (char *) buf, (int32_t) length, 0);
    }
}

int64_t sese::net::v2::IOContext::writeWithoutSSL(const void *buf, size_t length) noexcept {// NOLINT
    return ::send(socket, (const char *) buf, (int32_t) length, 0);
}

int64_t sese::net::v2::IOContext::readWithSSL(void *buf, size_t length) noexcept {// NOLINT
    return SSL_read((SSL *) this->ssl, buf, (int) length);
}

int64_t sese::net::v2::IOContext::writeWithSSL(const void *buf, size_t length) noexcept {// NOLINT
    return SSL_write((SSL *) this->ssl, buf, (int) length);
}

sese::net::v2::IOContext::IOContext(void *bioMethod, void *ssl) noexcept {// NOLINT
    if (bioMethod) {
        this->ssl = ssl;
        this->bio = BIO_new((BIO_METHOD *) bioMethod);
        BIO_set_data((BIO *) this->bio, this);
        BIO_set_init((BIO *) this->bio, 1);
        BIO_set_shutdown((BIO *) this->bio, 0);
    }
}

int64_t sese::net::v2::IOContext::read(void *buf, size_t length) noexcept {
    if (bio) {
        return readWithSSL(buf, length);
    } else {
        return readWithoutSSL(buf, length);
    }
}

int64_t sese::net::v2::IOContext::write(const void *buf, size_t length) noexcept {
    if (bio) {
        return writeWithSSL(buf, length);
    } else {
        return writeWithoutSSL(buf, length);
    }
}

void sese::net::v2::IOContext::close() noexcept {
    if (bio) {
        isClosed = true;
        SSL_shutdown((SSL *) ssl);
        SSL_free((SSL *) ssl);
        ::closesocket(socket);
    } else {
        isClosed = true;
        ::shutdown(socket, SD_BOTH);
        ::closesocket(socket);
    }
}

sese::net::v2::Server::Ptr sese::net::v2::Server::create(sese::net::v2::ServerOption *opt) noexcept {
    if (opt->isSSL) {
        if (opt->sslContext) {
            if (!opt->sslContext->authPrivateKey()) {
                return nullptr;
            }
        } else {
            return nullptr;
        }
    }

    auto family = opt->address->getRawAddress()->sa_family;
    socket_t sock = ::WSASocketW(
            family,
            SOCK_STREAM,
            0,
            nullptr,
            0,
            WSA_FLAG_OVERLAPPED
    );

    if (SOCKET_ERROR == sock) {
        return nullptr;
    }

    unsigned long ul = 1;
    if (SOCKET_ERROR == ioctlsocket(sock, FIONBIO, &ul)) {
        ::closesocket(sock);
        return nullptr;
    }

    if (SOCKET_ERROR == bind(sock, opt->address->getRawAddress(), opt->address->getRawAddressLength())) {
        ::closesocket(sock);
        return nullptr;
    }

    if (SOCKET_ERROR == listen(sock, SERVER_MAX_CONNECTION)) {
        ::closesocket(sock);
        return nullptr;
    }

    auto iocp = CreateIoCompletionPort(
            INVALID_HANDLE_VALUE,
            nullptr,
            0,
            (DWORD) opt->threads
    );
    if (INVALID_HANDLE_VALUE == iocp) {
        ::closesocket(sock);
        return nullptr;
    }

    auto server = new Server;
    server->option = opt;
    server->socket = sock;
    server->hIOCP = iocp;

    if (opt->isKeepAlive && opt->keepAlive > 0) {
        server->timer = Timer::create();
    }

    if (opt->isSSL) {
        auto method = BIO_meth_new(BIO_get_new_index() | BIO_TYPE_SOURCE_SINK, "bio_iocp");
        BIO_meth_set_ctrl(method, bio_iocp_ctrl);
        BIO_meth_set_read(method, bio_iocp_read);
        BIO_meth_set_write(method, bio_iocp_write);
        server->bioMethod = method;
    }

    for (int index = 0; index < opt->threads; index++) {
        server->threads.emplace_back(std::make_unique<Thread>([server]() { server->WindowsWorkerFunction(); }, "SERV" + std::to_string(index)));
    }

    return std::unique_ptr<sese::net::v2::Server>(server);
}

void sese::net::v2::Server::onConnect() noexcept {
    SOCKET client = ::accept(socket, nullptr, nullptr);
    if (SOCKET_ERROR == client) {
        return;
    }

    // SSL 握手
    ssl_st *clientSSL = nullptr;
    if (option->isSSL) {
        clientSSL = SSL_new((SSL_CTX *) option->sslContext->getContext());
        SSL_set_fd(clientSSL, (int) client);
        SSL_set_accept_state(clientSSL);

        while (true) {
            auto rt = SSL_do_handshake(clientSSL);
            if (rt <= 0) {
                // err is SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE
                auto err = SSL_get_error(clientSSL, rt);
                if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                    SSL_free(clientSSL);
                    closesocket(client);
                    clientSSL = nullptr;
                    break;
                }
            } else {
                break;
            }
        }
        if (clientSSL == nullptr) {
            return;
        }
    }

    unsigned long ul = 1;
    if (SOCKET_ERROR == ioctlsocket(client, FIONBIO, &ul)) {
        closesocket(client);
        return;
    }

    if (nullptr == CreateIoCompletionPort((HANDLE) client, hIOCP, 0, 0)) {
        closesocket(client);
        return;
    }

    auto ctx = new sese::net::v2::IOContext(bioMethod, clientSSL);
    ctx->socket = client;
#ifdef _DEBUG
    printf("NEW: %p\n", ctx);
#endif
    // 首次投递
    DWORD nBytes = MaxBufferSize;
    DWORD dwFlags = 0;
    int nRt = WSARecv(client, &ctx->wsaBuf, 1, &nBytes, &dwFlags, &(ctx->overlapped), nullptr);
    int e = WSAGetLastError();
    if (SOCKET_ERROR == nRt && ERROR_IO_PENDING != e) {
#ifdef _DEBUG
        printf("BAD: %p\n", ctx);
#endif
        ctx->close();
        delete ctx;
        return;
    }

    if (option->isKeepAlive && option->keepAlive > 0) {
        mutex.lock();
        taskMap[ctx] = timer->delay(
                [this, ctx]() {
                    mutex.lock();
                    auto iterator = taskMap.find(ctx);
                    if (iterator != taskMap.end()) {
#ifdef _DEBUG
                        printf("CLOSE: %p - TIMEOUT\n", ctx);
#endif
                        taskMap.erase(iterator);
                        ctx->close();
                        delete ctx;
                    }
                    mutex.unlock();
                },
                option->keepAlive, false
        );
        mutex.unlock();
    }
}

void sese::net::v2::Server::loop() noexcept {
    for (auto &th: threads) {
        th->start();
    }

    while (true) {
        if (isShutdown) break;
        onConnect();
    }
}

void sese::net::v2::Server::shutdown() noexcept {
    void *lpCompletionKey = nullptr;
    isShutdown = true;
    for (auto i = 0; i < option->threads; ++i) {
        PostQueuedCompletionStatus(
                hIOCP,
                -1,
                (ULONG_PTR) lpCompletionKey,
                nullptr
        );
    }

    for (auto &th: threads) {
        th->join();
    }

    if (option->isKeepAlive && option->keepAlive > 0) {
        timer->shutdown();

        // mutex.lock();
        for (auto &pair: taskMap) {
            pair.first->close();
#ifdef _DEBUG
            printf("CLOSE: %p - SHUT\n", pair.first);
#endif
            delete pair.first;
        }
        taskMap.clear();
        // mutex.unlock();
    }

    if (option->isSSL) {
        BIO_meth_free((BIO_METHOD *) bioMethod);
    }
}

void sese::net::v2::Server::WindowsWorkerFunction() noexcept {
    IOContext *ctx = nullptr;
    DWORD lpNumberOfBytesTransferred = 0;
    void *lpCompletionKey = nullptr;
    DWORD dwFlags = 0;
    DWORD nBytes = MaxBufferSize;

    while (true) {
        BOOL bRt = GetQueuedCompletionStatus(
                hIOCP,
                &lpNumberOfBytesTransferred,
                (PULONG_PTR) &lpCompletionKey,
                (LPOVERLAPPED *) &ctx,
                INFINITE
        );

        if (!bRt) continue;
        if (lpNumberOfBytesTransferred == -1) break;
        if (lpNumberOfBytesTransferred == 0) continue;
        if (lpNumberOfBytesTransferred == MaxBufferSize) {
#ifdef _DEBUG
            printf("BAD: %p\n", ctx);
#endif
            if (option->isKeepAlive && option->keepAlive > 0) {
                mutex.lock();
                auto iterator = taskMap.find(ctx);
                if (iterator != taskMap.end()) {
                    iterator->second->cancel();
                }
                mutex.unlock();
            }
            ctx->close();
            delete ctx;
            continue;
        }
        ctx->nBytes = lpNumberOfBytesTransferred;

        // 只处理首次读取事件
        int nRt = WSARecv(ctx->socket, &ctx->wsaBuf, 1, &nBytes, &dwFlags, &(ctx->overlapped), nullptr);
        int e = WSAGetLastError();
        if (SOCKET_ERROR == nRt && ERROR_IO_PENDING != e) {
#ifdef _DEBUG
            printf("BAD: %p\n", ctx);
#endif
            ctx->close();
            delete ctx;
            continue;
        }

#ifdef _DEBUG
        printf("RECV: %p\n", ctx);// NOLINT
#endif
        // 触发读事件，先取消原有计时
        if (option->isKeepAlive && option->keepAlive > 0) {
            mutex.lock();
            auto iterator = taskMap.find(ctx);
            if (iterator != taskMap.end()) {
                auto task = iterator->second;
                iterator->second = nullptr;
                // taskMap.erase(ctx);
                taskMap.erase(iterator);
                mutex.unlock();
                task->cancel();
            } else {
                mutex.unlock();
            }
        }

        // 回调函数调用
        auto isHandle = option->beforeHandle(ctx);
        if (isHandle) {
            option->onHandle(ctx);
        }

        // 启用长连接并且当前连接尚未关闭，重新计时
        if (option->isKeepAlive && option->keepAlive > 0 && !ctx->isClosed) {
            // 重新提交
            ctx->nRead = 0;
            ctx->nBytes = 0;
            nRt = WSARecv(ctx->socket, &ctx->wsaBuf, 1, &nBytes, &dwFlags, &(ctx->overlapped), nullptr);
            e = WSAGetLastError();
            if (SOCKET_ERROR == nRt && ERROR_IO_PENDING != e) {
#ifdef _DEBUG
                printf("BAD: %p\n", ctx);
#endif
                ctx->close();
                delete ctx;
                continue;
            } else {
#ifdef _DEBUG
                printf("POST: %p\n", ctx);
#endif
                // 提交无误才加入定时器
                mutex.lock();
                taskMap[ctx] = timer->delay(
                        [this, ctx]() {
                            mutex.lock();
                            auto iterator = taskMap.find(ctx);
                            if (iterator != taskMap.end()) {
#ifdef _DEBUG
                                printf("CLOSE: %p - TIMEOUT(RE)\n", ctx);
#endif
                                taskMap.erase(iterator);
                                ctx->close();
                                delete ctx;
                            }
                            mutex.unlock();
                        },
                        option->keepAlive, false
                );
                mutex.unlock();
                continue;
            }
        }
        // 启用了自动关闭且当前连接尚未关闭
        else if (option->autoClose && !ctx->isClosed) {
#ifdef _DEBUG
            printf("CLOSE: %p - AUTO\n", ctx);
#endif
            ctx->close();
            delete ctx;
            continue;
        }
    }
}