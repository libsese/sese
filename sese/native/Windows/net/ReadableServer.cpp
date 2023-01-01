#include "sese/net/ReadableServer.h"

int64_t sese::net::IOContext::read(void *dst, size_t length) {
    // 缓冲区内有未读字节
    if (this->nRead < this->nBytes) {
        // 缓冲区够用
        if (this->nBytes - this->nRead > length) {
            memcpy(dst, this->buffer + this->nRead, length);
            this->nRead += length;
            return (int64_t) length;
        }
        // 缓冲区不够用
        // 直接返回当前剩余部分
        else {
            memcpy(dst, this->buffer + this->nRead, this->nBytes - this->nRead);
            this->nRead = this->nBytes;
            return this->nBytes - this->nRead;
        }
    }
    // 缓冲区已空
    else {
        return ::recv(socket, (char *) dst, (int32_t) length, 0);
    }
}

int64_t sese::net::IOContext::write(const void *src, size_t length) const {
    return ::send(socket, (const char *) src, (int32_t) length, 0);
}

void sese::net::IOContext::close() {
    isClosed = true;
    ::shutdown(socket, SD_BOTH);
    ::closesocket(socket);
}

sese::net::ReadableServer::Ptr sese::net::ReadableServer::create(const Handler &handler, size_t threads) noexcept {
    HANDLE hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, threads);
    if (INVALID_HANDLE_VALUE == hIOCP) {
        return nullptr;
    }

    auto server = new ReadableServer;
    for (auto i = 0; i < threads; ++i) {
        server->threadGroup.push_back(std::make_unique<Thread>([server]() { server->WindowsWorkerFunction(); }, "IOCP_READ" + std::to_string(i)));
    }
    for (Thread::Ptr &th: server->threadGroup) {
        th->start();
    }
    server->hIOCP = hIOCP;
    server->handler = handler;
    return std::unique_ptr<ReadableServer>(server);
}

bool sese::net::ReadableServer::add(socket_t socket) noexcept {
    unsigned long ul = 1;
    if (SOCKET_ERROR == ioctlsocket(socket, FIONBIO, &ul)) {
        return false;
    }

    // 添加至 IOCP
    if (nullptr == CreateIoCompletionPort((HANDLE) socket, this->hIOCP, 0, 0)) {
        return false;
    }

    auto ctx = new sese::net::IOContext;
    ctx->socket = socket;
#ifdef _DEBUG
    printf("NEW: %p\n", ctx);
#endif

    // 首次提交
    DWORD nBytes = MaxBufferSize;
    DWORD dwFlags = 0;
    int nRt = WSARecv(socket, &ctx->wsaBuf, 1, &nBytes, &dwFlags, &ctx->overlapped, nullptr);
    auto e = WSAGetLastError();
    if (SOCKET_ERROR == nRt && ERROR_IO_PENDING != e) {
#ifdef _DEBUG
        printf("CLOSE: %p\n", ctx);
#endif
        delete ctx;
        return false;
    }

    mutex.lock();
    map[socket] = ctx;
    mutex.unlock();
    return true;
}

void sese::net::ReadableServer::shutdown() noexcept {
    void *lpCompletionKey = nullptr;
    for (auto i = 0; i < threadGroup.size(); i++) {
        PostQueuedCompletionStatus(hIOCP, -1, (ULONG_PTR) lpCompletionKey, nullptr);
    }
    for (auto &pair: map) {
        ::shutdown(pair.first, SD_BOTH);
        closesocket(pair.first);
#ifdef _DEBUG
        printf("CLOSE: %p\n", pair.second);
#endif
        delete pair.second;
    }
    for (auto &thread: threadGroup) {
        thread->join();
    }
}

void sese::net::ReadableServer::WindowsWorkerFunction() noexcept {
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
        ctx->nBytes = lpNumberOfBytesTransferred;

        int nRt = WSARecv(
                ctx->socket,
                &ctx->wsaBuf,
                1,
                &nBytes,
                &dwFlags,
                &(ctx->overlapped),
                nullptr
        );
        auto e = WSAGetLastError();
        if (SOCKET_ERROR == nRt && ERROR_IO_PENDING != e) {
#ifdef _DEBUG
            printf("CLOSE: %p\n", ctx);
#endif
            mutex.lock();
            auto iterator = map.find(ctx->socket);
            if (iterator != map.end()) {
                map.erase(iterator);
            }
            mutex.unlock();
            closesocket(ctx->socket);
            delete ctx;
            ctx = nullptr;
            continue;
        } else {
#ifdef _DEBUG
            printf("RECV: %p\n", ctx);
#endif
            handler(ctx);
            if (ctx->isClosed) {
                // 不需要保留连接，已主动关闭
#ifdef _DEBUG
                printf("CLOSE: %p\n", ctx);
#endif
                mutex.lock();
                auto iterator = map.find(ctx->socket);
                if (iterator != map.end()) {
                    map.erase(iterator);
                }
                mutex.unlock();
                delete ctx;
            } else {
                // 再次提交读取
                ctx->nRead = 0;
                ctx->nBytes = 0;
                nRt = WSARecv(ctx->socket, &ctx->wsaBuf, 1, &nBytes, &dwFlags, &ctx->overlapped, nullptr);
                e = WSAGetLastError();
                if (SOCKET_ERROR == nRt && ERROR_IO_PENDING != e) {
                    mutex.lock();
                    auto iterator = map.find(ctx->socket);
                    if (iterator != map.end()) {
                        map.erase(iterator);
                    }
                    mutex.unlock();
                    closesocket(ctx->socket);
                    delete ctx;
                    ctx = nullptr;
                    continue;
                } else {
#ifdef _DEBUG
                    printf("POST: %p\n", ctx);
#endif
                }
            }
        }
    }
}