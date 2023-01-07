#pragma once

#include "sese/net/TcpServer.h"
#include "sese/security/SSLContext.h"

namespace sese::security {

    struct API IOContext final {
        int64_t read(void *buf, size_t length);
        int64_t write(const void *buffer, size_t length);
        void close();

#ifdef _WIN32
        IOContext() noexcept;
        WSAOVERLAPPED overlapped{};
        WSABUF wsaBuf{MaxBufferSize, buffer};
        CHAR buffer[MaxBufferSize]{};
        DWORD nBytes = 0;
        DWORD nRead = 0;
        void *bio = nullptr;
        void setSSL(void *ssl) noexcept;
#endif
        bool isClosed = false;
        socket_t socket = -1;
        void *ssl = nullptr;
    };

    class API SecurityTcpServer final : public Noncopyable {
    public:
        using Ptr = std::unique_ptr<SecurityTcpServer>;
#ifdef _WIN32
        using Map = std::map<IOContext *, TimerTask::Ptr>;
#else
        using Map = std::map<socket_t, TimerTask::Ptr>;
#endif
        static SecurityTcpServer::Ptr create(const IPAddress::Ptr &ipAddress, size_t threads, size_t keepAlive, SSLContext::Ptr &ctx) noexcept;

        void loopWith(const std::function<void(IOContext *)> &handler) noexcept;
        void shutdown() noexcept;
        [[nodiscard]] size_t getKeepAlive() const { return keepAlive; }

    private:
        SecurityTcpServer() noexcept = default;

        std::atomic_bool isShutdown = false;
        std::mutex mutex;
        Map taskMap;
        size_t keepAlive = 0;
        Timer::Ptr timer = nullptr;

#ifdef __linux__
    private:
        void closeCallback(socket_t socket) noexcept;

        int32_t epollFd = -1;
        socket_t sockFd = -1;
        epoll_event events[MaxEvents]{};
        ThreadPool::Ptr threadPool = nullptr;
        ObjectPool<IOContext>::Ptr ioContextPool = nullptr;
#endif
#ifdef __APPLE__
    private:
        void closeCallback(socket_t socket) noexcept;

        int32_t kqueueFd = -1;
        socket_t sockFd = -1;
        KEvent events[MaxEvents]{};
        ThreadPool::Ptr threadPool = nullptr;
        ObjectPool<IOContext>::Ptr ioContextPool = nullptr;
#endif
#ifdef _WIN32
    private:
        void WindowsCloseCallback(IOContext *contextWrapper) noexcept;
        void WindowsWorkerFunction() noexcept;

        SSLContext::Ptr ctx;
        SOCKET listenSock = INVALID_SOCKET;
        HANDLE hIOCP = INVALID_HANDLE_VALUE;
        size_t threads = 0;
        std::vector<Thread::Ptr> threadGroup;
        std::function<void(IOContext *)> handler;
#endif
    };
}// namespace sese::security