#pragma once
#include <sese/net/Socket.h>
#include <sese/thread/ThreadPool.h>

namespace sese {
    class API IOContext;
    class API TcpServer;
}// namespace sese

enum class OperationType {
    Send,
    Recv,
    Null
};

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

private:
    WSABUF wsaBuf{};
    OVERLAPPED overlapped{};
    Socket::Ptr socket{};
    OperationType operationType{};
    WSAEVENT event{};
};

class sese::TcpServer {
public:
    bool init(const IPAddress::Ptr &ipAddress, size_t threads = 4) noexcept;
    void loopWith(const std::function<void(IOContext *)> &handler);
    void shutdown();

private:
    void workerProc4WindowsIOCP();
    HANDLE iocpHandle = INVALID_HANDLE_VALUE;
    size_t threads{}; // Windows 工作线程数

protected:
    ThreadPool::Ptr threadPool{};
    Socket::Ptr socket{};
    std::atomic_bool isShutdown{true};
};