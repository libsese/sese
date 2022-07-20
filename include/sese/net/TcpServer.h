/**
 * @file TcpServer.h
 * @author kaoru
 * @date 2022年5月5日
 * @brief TCP Server 类
 */
#pragma once
#include <sese/net/Socket.h>
#include <sese/thread/ThreadPool.h>
#include <sese/Noncopyable.h>
#include <sese/Timer.h>
#include <sese/ObjectPool.h>
#include <map>

#ifdef __linux__
#include <sys/epoll.h>
#define MaxEvents 64
#define EpollCreate epoll_create1
#define EpollWait epoll_wait
#define EpollCtl epoll_ctl
#define EpollEvent epoll_event
#endif
#ifdef __APPLE__
#include <sys/event.h>
#define MaxEvents 64
using KEvent = struct kevent;
#endif

namespace sese {
#ifdef _WIN32
#define MaxBufferSize 1024
    enum class Operation {
        Accept = 1,
        Read = 2,
        Write = 3,
        Close = 4,
        Null = 0
    };
#endif

    class TcpServer;
    class API IOContext final : public Stream {
    public:
        friend class TcpServer;
        IOContext() : Stream() {}
        int64_t read(void *buffer, size_t length) override;
        int64_t write(const void *buffer, size_t length) override;
        void close() override;

    private:
#ifdef _WIN32
        WSABUF wsaBuf {MaxBufferSize, buffer};
        char useless[13];
        char buffer[MaxBufferSize]{};
        OVERLAPPED overlapped{};
        Operation operation = Operation::Null;
        DWORD nBytes = 0;
#endif
        bool isClosed = false;
        socket_t socket = -1;
    };

    class API TcpServer final : public Noncopyable {
    public:
        using Ptr = std::unique_ptr<TcpServer>;
        using Map = std::map<socket_t, TimerTask::Ptr>;

        static TcpServer::Ptr create(const IPAddress::Ptr &ipAddress, size_t threads, size_t keepAlive = SERVER_KEEP_ALIVE_DURATION) noexcept;
        void loopWith(const std::function<void(IOContext *)> &handler) noexcept;
        void shutdown() noexcept;

    private:
        TcpServer() noexcept = default;
        void closeCallback(socket_t socket) noexcept;

        std::atomic_bool isShutdown = false;
        ThreadPool::Ptr threadPool = nullptr;
        std::mutex mutex;
        Map taskMap;
        size_t keepAlive = 0;
        Timer::Ptr timer = nullptr;
        ObjectPool<IOContext>::Ptr ioContextPool = nullptr;

#ifdef __linux__
    private:
        int32_t epollFd = -1;
        socket_t sockFd = -1;
        epoll_event events[MaxEvents]{};
#endif
#ifdef __APPLE__
    private:
        int32_t kqueueFd = -1;
        socket_t sockFd = -1;
        KEvent events[MaxEvents]{};
#endif
#ifdef _WIN32
    private:
        void postRecv(SOCKET socket) noexcept;
        void forwardFunction() noexcept;

        SOCKET listenSock = INVALID_SOCKET;
        HANDLE hIOCP = INVALID_HANDLE_VALUE;
        std::vector<Thread::Ptr> threadArray;
#endif
    };
}// namespace sese