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

    int64_t recv() noexcept;
    int64_t send() noexcept;
    void close() noexcept;

public:
    char buffer[SERVER_MAX_BUFFER_SIZE]{};
private:
    OVERLAPPED overlapped{};
    SOCKET socket = INVALID_SOCKET;
    WSABUF wsaBuf{SERVER_MAX_BUFFER_SIZE, buffer};
    DWORD totalBytes = 0;
    DWORD dealBytes = 0;
    OperationType operationType = OperationType::Null;
};

class sese::TcpServer {
public:
    bool init(const IPAddress::Ptr &ipAddress) noexcept;
    void loopWith(const std::function<void(IOContext *)> &handler);
    void shutdown();

private:
    void workerProc4WindowsIOCP();
    std::vector<Thread::Ptr> threadGroup;

protected:
    HANDLE iocpHandle = INVALID_HANDLE_VALUE;
    Socket::Ptr socket = nullptr;
    std::atomic_bool isShutdown{false};
};