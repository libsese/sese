#pragma once
#include <sese/net/Socket.h>
#include <sese/thread/ThreadPool.h>

#ifdef __linux__
// Linux Native Only
#include <sys/epoll.h>
#endif

namespace sese {
    class API IOContext;
    class API TcpServer;
}// namespace sese

#ifdef _WIN32
// Windows Native Only
/// 操作类型
enum class OperationType {
    Send,
    Recv,
    Null
};
#endif

class sese::IOContext {
public:
    friend class sese::TcpServer;

    IOContext() noexcept;
    ~IOContext() noexcept;
    int64_t recv(void *buffer, size_t size) noexcept;
    int64_t send(const void *buffer, size_t size) noexcept;
    int32_t shutdown(Socket::ShutdownMode mode) const noexcept; // NOLINT
    void close() const noexcept;
    [[nodiscard]] const IPAddress::Ptr &getClientAddress() const noexcept;

#ifdef _WIN32
    // Windows Native Only
private:
    WSABUF wsaBuf{};
    OVERLAPPED overlapped{};
    Socket::Ptr socket{};
    OperationType operationType{};
    WSAEVENT event{};
#endif
#ifdef __linux__
    // Linux Native Only
    Socket::Ptr socket{};
#endif
};

class sese::TcpServer {
public:
    bool init(const IPAddress::Ptr &ipAddress, size_t threads = SERVER_DEFAULT_THREADS) noexcept;
    void loopWith(const std::function<void(IOContext *)> &handler);
    void shutdown();

#ifdef _WIN32
    // Windows Native Only
private:
    void workerProc4WindowsIOCP();
    HANDLE iocpHandle = INVALID_HANDLE_VALUE;
    size_t threads{}; // Windows 工作线程数
#endif
#ifdef __linux__
    // Linux Native Only
    int32_t epollFd = -1;
    epoll_event events[64];
#endif

protected:
    ThreadPool::Ptr threadPool{};
    Socket::Ptr socket{};
    std::atomic_bool isShutdown{true};
};