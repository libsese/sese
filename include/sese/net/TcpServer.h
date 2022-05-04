#pragma once
#include <sese/net/Socket.h>
#include <sese/thread/Thread.h>

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
    bool init(const IPAddress::Ptr &ipAddress) noexcept;
    void loopWith(const std::function<void(IOContext *)> &handler);
    void shutdown();

private:
    void workerProc4WindowsIOCP();
    std::vector<Thread::Ptr> threadGroup;
    HANDLE iocpHandle = INVALID_HANDLE_VALUE;

protected:
    Socket::Ptr socket{};
    std::atomic_bool isShutdown{false};
};