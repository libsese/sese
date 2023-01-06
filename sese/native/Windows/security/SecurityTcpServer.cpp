#include "sese/security/SecurityTcpServer.h"
#include "openssl/ssl.h"

int64_t sese::security::IOContext::read(void *buf, size_t length) {
    // 缓冲区内有未读字节
    if (this->nRead < this->nBytes) {
        // 缓冲区够用
        if (this->nBytes - this->nRead > length) {
            BIO *bio = SSL_get_rbio((SSL *) ssl);
            BIO_write(bio, this->buffer + this->nBytes, (int) length);
            SSL_read((SSL *) ssl, buf, (int) length);
            this->nRead += (DWORD) length;
            return (int64_t) length;
        }
        // 缓冲区不够用
        // 直接返回当前剩余部分
        else {
            BIO *bio = SSL_get_rbio((SSL *) ssl);
            BIO_write(bio, this->buffer + this->nRead, (int) (this->nBytes - this->nRead));
            SSL_read((SSL *) ssl, buf, (int) (this->nBytes - this->nRead));
            this->nRead = this->nBytes;
            return this->nBytes - this->nRead;
        }
    }
    // 缓冲区已空
    else {
        // return ::recv(socket, (char *) buffer, (int32_t) length, 0);
        return SSL_read((SSL *) ssl, buf, (int) length);
    }
}

int64_t sese::security::IOContext::write(const void *buf, size_t length) {
    return SSL_write((SSL *) ssl, buf, (int) length);
}

void sese::security::IOContext::close() {
    SSL_shutdown((SSL *) ssl);
    SSL_free((SSL *) ssl);
    closesocket(socket);
}

sese::security::SecurityTcpServer::Ptr sese::security::SecurityTcpServer::create(const IPAddress::Ptr &ipAddress, size_t threads, size_t keepAlive, SSLContext::Ptr &ctx) noexcept {
    auto family = ipAddress->getRawAddress()->sa_family;
    socket_t sockFd = ::WSASocketW(family, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
    if (-1 == sockFd) {
        return nullptr;
    }

    unsigned long ul = 1;
    if (SOCKET_ERROR == ioctlsocket(sockFd, FIONBIO, &ul)) {
        closesocket(sockFd);
        return nullptr;
    }

    if (SOCKET_ERROR == bind(sockFd, ipAddress->getRawAddress(), ipAddress->getRawAddressLength())) {
        closesocket(sockFd);
        return nullptr;
    }

    if (SOCKET_ERROR == listen(sockFd, SERVER_MAX_CONNECTION)) {
        closesocket(sockFd);
        return nullptr;
    }

    // 验证私钥
    if (!ctx->authPrivateKey()) {
        closesocket(sockFd);
        return nullptr;
    }

    HANDLE hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, (DWORD) threads);
    if (INVALID_HANDLE_VALUE == hIOCP) {
        closesocket(sockFd);
        return nullptr;
    }

    auto server = new SecurityTcpServer;

    server->threads = threads;
    for (size_t index = 0; index < threads; index++) {
        server->threadGroup.emplace_back(std::make_unique<Thread>([server]() { server->WindowsWorkerFunction(); }, "IOCP" + std::to_string(index)));
    }

    server->ctx = ctx;
    server->listenSock = sockFd;
    server->hIOCP = hIOCP;
    server->timer = Timer::create();
    server->keepAlive = keepAlive;
    return std::unique_ptr<SecurityTcpServer>(server);
}

void sese::security::SecurityTcpServer::loopWith(const std::function<void(IOContext *)> &handler) noexcept {
    this->handler = handler;

    for (Thread::Ptr &th: threadGroup) {
        th->start();
    }

    while (true) {
        if (isShutdown) break;

        SOCKET client = ::accept(listenSock, nullptr, nullptr);
        if (SOCKET_ERROR == client) continue;

        auto clientSSL = SSL_new((SSL_CTX *) ctx->getContext());
        SSL_set_fd(clientSSL, (int) client);
        SSL_set_accept_state(clientSSL);
        auto rt = SSL_do_handshake(clientSSL);
        if (rt <= 0) {
            //todo err is SSL_ERROR_WANT_READ
            auto err = SSL_get_error(clientSSL, rt);
            SSL_free(clientSSL);
            closesocket(client);
            continue;
        }

        unsigned long ul = 1;
        if (SOCKET_ERROR == ioctlsocket(client, FIONBIO, &ul)) {
            closesocket(client);
            continue;
        }

        if (nullptr == CreateIoCompletionPort((HANDLE) client, hIOCP, 0, 0)) {
            closesocket(client);
            continue;
        }

        // 首次接入
        auto ioContext = new IOContext;
        ioContext->socket = client;
        ioContext->ssl = clientSSL;
#ifdef _DEBUG
        printf("NEW: %p\n", ioContext);
#endif

        if (0 != keepAlive) {
            mutex.lock();
            taskMap[ioContext] = timer->delay([this, ioContext]() { this->WindowsCloseCallback(ioContext); }, (int64_t) keepAlive, false);
            mutex.unlock();
        }

        DWORD nBytes = MaxBufferSize;
        DWORD dwFlags = 0;
        int nRt = WSARecv(client, &ioContext->wsaBuf, 1, &nBytes, &dwFlags, &ioContext->overlapped, nullptr);
        auto e = WSAGetLastError();
        if (SOCKET_ERROR == nRt && ERROR_IO_PENDING != e) {
            if (keepAlive > 0) {
                WindowsCloseCallback(ioContext);
            } else {
#ifdef _DEBUG
                printf("CLOSE: %p\n", ioContext);
#endif
                closesocket(client);
                delete ioContext;
            }
        }
    }
}

void sese::security::SecurityTcpServer::shutdown() noexcept {
    void *lpCompletionKey = nullptr;
    isShutdown = true;
    for (auto i = 0; i < threads; i++) {
        PostQueuedCompletionStatus(hIOCP, -1, (ULONG_PTR) lpCompletionKey, nullptr);
    }
    timer->shutdown();
    for (auto &pair: taskMap) {
        SSL_shutdown((SSL *) pair.first->ssl);
        SSL_free((SSL *) pair.first->ssl);
        closesocket(pair.first->socket);
#ifdef _DEBUG
        printf("CLOSE: %p\n", pair.first);
#endif
        delete pair.first;
    }
    for (auto &thread: threadGroup) {
        thread->join();
    }
}

void sese::security::SecurityTcpServer::WindowsCloseCallback(IOContext *ioContext) noexcept {
#ifdef _DEBUG
    printf("CLOSE: %p\n", ioContext);
#endif
    mutex.lock();
    taskMap.erase(ioContext);
    mutex.unlock();
    SSL_shutdown((SSL *) ioContext->ssl);
    SSL_free((SSL *) ioContext->ssl);
    closesocket(ioContext->socket);
    delete ioContext;
}

void sese::security::SecurityTcpServer::WindowsWorkerFunction() noexcept {
    IOContext *ioContext = nullptr;
    DWORD lpNumberOfBytesTransferred = 0;
    void *lpCompletionKey = nullptr;

    DWORD dwFlags = 0;
    DWORD nBytes = MaxBufferSize;

    while (true) {
        BOOL bRt = GetQueuedCompletionStatus(
                hIOCP,
                &lpNumberOfBytesTransferred,
                (PULONG_PTR) &lpCompletionKey,
                (LPOVERLAPPED *) &ioContext,
                INFINITE
        );

        if (!bRt) continue;
        if (lpNumberOfBytesTransferred == -1) break;
        if (lpNumberOfBytesTransferred == 0) continue;
        if (lpNumberOfBytesTransferred == MaxBufferSize) {
#ifdef _DEBUG
            printf("BAD: %p\n", ioContext);
#endif
            SSL_shutdown((SSL *) ioContext->ssl);
            SSL_free((SSL *) ioContext->ssl);
            closesocket(ioContext->socket);
            delete ioContext;
            ioContext = nullptr;
            continue;
        }
        ioContext->nBytes = lpNumberOfBytesTransferred;

        // 只处理首次读事件
        int nRt = WSARecv(
                ioContext->socket,
                &ioContext->wsaBuf,
                1,
                &nBytes,
                &dwFlags,
                &(ioContext->overlapped),
                nullptr
        );
        auto e = WSAGetLastError();
        if (SOCKET_ERROR == nRt && ERROR_IO_PENDING != e) {
            // 读取发生错误
#ifdef _DEBUG
            printf("CLOSE: %p\n", ioContext);
#endif
            // SSL_shutdown((SSL *) ioContext->ssl);
            SSL_free((SSL *) ioContext->ssl);
            closesocket(ioContext->socket);
            delete ioContext;
            ioContext = nullptr;
            continue;
        } else {
            // 已经读取到数据 - 触发首次事件
            // ...
#ifdef _DEBUG
            printf("RECV: %p\n", ioContext);
#endif
            auto client = ioContext->socket;
            if (0 != keepAlive) {
                mutex.lock();
                auto iterator = taskMap.find(ioContext);
                if (iterator != taskMap.end()) {
                    auto task = iterator->second;
                    taskMap.erase(ioContext);
                    mutex.unlock();
                    task->cancel();
                } else {
                    mutex.unlock();
                }
            }

            handler(ioContext);

            if (ioContext->isClosed) {
                // 不需要保留连接，已主动关闭
#ifdef _DEBUG
                printf("CLOSE: %p\n", ioContext);
#endif
                delete ioContext;
            } else {
                if (0 == keepAlive) {
#ifdef _DEBUG
                    printf("CLOSE: %p\n", ioContext);
#endif
                    SSL_shutdown((SSL *) ioContext->ssl);
                    SSL_free((SSL *) ioContext->ssl);
                    ioContext->close();
                    delete ioContext;
                } else {
                    // 继续计时
                    mutex.lock();
                    taskMap[ioContext] = timer->delay([this, ioContext]() { this->WindowsCloseCallback(ioContext); }, (int64_t) keepAlive, false);
                    mutex.unlock();

                    // 再次提交读取
                    ioContext->nRead = 0;
                    ioContext->nBytes = 0;
                    nRt = WSARecv(client, &ioContext->wsaBuf, 1, &nBytes, &dwFlags, &ioContext->overlapped, nullptr);
                    e = WSAGetLastError();
                    if (SOCKET_ERROR == nRt && ERROR_IO_PENDING != e) {
                        this->WindowsCloseCallback(ioContext);
                    } else {
#ifdef _DEBUG
                        printf("POST: %p\n", ioContext);
#endif
                    }
                }
            }
        }
    }
}