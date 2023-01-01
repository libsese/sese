/**
 * \file ReadableServer.h
 * \author kaoru
 * \date 2023.01.01
 * \version 0.1
 * \brief 可读处理器，实现与 TcpServer 基本相同，少了 accept 和定时相关操作
 */
#pragma once

#include "sese/net/TcpServer.h"

namespace sese::net {
    struct API IOContext;
    class API ReadableServer;
}// namespace sese::net

struct API sese::net::IOContext final {
    int64_t read(void *dst, size_t length);
    int64_t write(const void *buffer, size_t length) const;
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

class API sese::net::ReadableServer final : public Noncopyable {
public:
    using Ptr = std::unique_ptr<ReadableServer>;
    using Handler = std::function<void(IOContext *)>;

    static ReadableServer::Ptr create(const Handler &handler, size_t threads) noexcept;
    bool add(socket_t socket) noexcept;
    void shutdown() noexcept;

private:
    ReadableServer() = default;
    Handler handler;
#ifdef WIN32
    std::mutex mutex;
    std::map<socket_t, IOContext *> map;
    void WindowsWorkerFunction() noexcept;
    HANDLE hIOCP = INVALID_HANDLE_VALUE;
    std::vector<Thread::Ptr> threadGroup;
#endif
};