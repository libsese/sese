/**
 * @file TcpServer.h
 * @author kaoru
 * @date 2022年7月22日
 * @brief TCP Server 类
 * @version 0.3
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
        Accept,
        Read,
        Write,
        Close,
        Null
    };
#endif

    class TcpServer;
    struct API IOContext final {
    public:
        friend class TcpServer;
        int64_t read(void *buffer, size_t length);
        int64_t write(const void *buffer, size_t length);
        void close();
        void detach();

#ifdef _WIN32
        WSAOVERLAPPED overlapped{};
        WSABUF wsaBuf{MaxBufferSize, buffer};
        CHAR buffer[MaxBufferSize]{};
        Operation operation = Operation::Null;
        DWORD nBytes = 0;
        DWORD nRead = 0;
#endif
        bool isClosed = false;
        bool isDetach = false;
        socket_t socket = -1;
    };

    class API TcpServer final : public Noncopyable {
    public:
        using Ptr = std::unique_ptr<TcpServer>;
#ifdef _WIN32
        using Map = std::map<IOContext *, TimerTask::Ptr>;
#else
        using Map = std::map<socket_t, TimerTask::Ptr>;
#endif

        static TcpServer::Ptr create(const IPAddress::Ptr &ipAddress, size_t threads, size_t keepAlive = SERVER_KEEP_ALIVE_DURATION) noexcept;
        static TcpServer::Ptr create(const Socket::Ptr &listenSocket, size_t threads, size_t keepAlive = SERVER_KEEP_ALIVE_DURATION) noexcept;
        void loopWith(const std::function<void(IOContext *)> &handler) noexcept;
        void shutdown() noexcept;
        [[nodiscard]] size_t getKeepAlive() const { return keepAlive; }

    private:
        TcpServer() noexcept = default;

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

        SOCKET listenSock = INVALID_SOCKET;
        HANDLE hIOCP = INVALID_HANDLE_VALUE;
        size_t threads = 0;
        std::vector<Thread::Ptr> threadGroup;
        std::function<void(IOContext *)> handler;
#endif
    };
}// namespace sese