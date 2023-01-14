/**
 * \file ReadableServer.h
 * \author kaoru
 * \date 2023.01.01
 * \version 0.1
 * \brief 可读处理器，实现与 TcpServer 基本相同，少了 accept 和定时相关操作
 */
#pragma once

#include "sese/net/TcpServer.h"
#include "sese/util/ObjectPool.h"

namespace sese::net {
    struct API IOContext;
    class API ReadableServer;
}// namespace sese::net

struct API [[deprecated("此实现缺少 SSL 支持")]] sese::net::IOContext final {
    int64_t read(void *dst, size_t length);
    int64_t write(const void *src, size_t length) const;
    void close();

#ifdef _WIN32
    WSAOVERLAPPED overlapped{};
    WSABUF wsaBuf{MaxBufferSize, buffer};
    CHAR buffer[MaxBufferSize]{};
    DWORD nBytes = 0;
    DWORD nRead = 0;
#endif
    bool isClosed = false;
    socket_t socket = -1;
};


class API [[deprecated("此实现缺少 SSL 支持")]] sese::net::ReadableServer final : public Noncopyable {
public:
    using Ptr = std::unique_ptr<ReadableServer>;
    using Handler = std::function<void(IOContext *)>;

    static ReadableServer::Ptr create(const Handler &handler, size_t threads) noexcept;
    bool add(socket_t socket) noexcept;
    void shutdown() noexcept;

private:
    ReadableServer() = default;
    Handler handler;
#ifdef __APPLE__
    void DarwinWorkerFunction() noexcept;
    std::atomic_bool isShutdown = false;
    int32_t kqueueFd = -1;
    KEvent events[MaxEvents]{};
    Thread::Ptr workerThread = nullptr;
    ThreadPool::Ptr threadPool = nullptr;
    ObjectPool<IOContext>::Ptr ioContextPool = nullptr;
#endif
#ifdef __linux__
    void LinuxWorkerFunction() noexcept;
    std::atomic_bool isShutdown = false;
    int32_t epollFd = -1;
    epoll_event events[MaxEvents]{};
    Thread::Ptr workerThread = nullptr;
    ThreadPool::Ptr threadPool = nullptr;
    ObjectPool<IOContext>::Ptr ioContextPool = nullptr;
#endif
#ifdef WIN32
    std::mutex mutex;
    std::map<socket_t, IOContext *> map;
    void WindowsWorkerFunction() noexcept;
    HANDLE hIOCP = INVALID_HANDLE_VALUE;
    std::vector<Thread::Ptr> threadGroup;
#endif
};