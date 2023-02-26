#pragma once

#include "sese/net/Socket.h"
#include "sese/security/SSLContext.h"
#include "sese/thread/ThreadPool.h"
#include "sese/util/Timer.h"

#include <map>
#include <memory>

#ifdef WIN32
#define MaxBufferSize 8192
#include <vector>
#elif __linux__
#define MaxEventSize 64
#include <sys/epoll.h>
#elif __APPLE__
#define MaxEventSize 64
#include <sys/event.h>
#endif

namespace sese::net::v2 {

    struct IOContext {
        int64_t read(void *buffer, size_t length) noexcept;
        int64_t write(const void *buffer, size_t length) noexcept;
        void close() noexcept;

#ifdef WIN32
        IOContext(void *bioMethod, void *ssl) noexcept;
        WSAOVERLAPPED overlapped{};
        WSABUF wsaBuf{MaxBufferSize, buffer};
        CHAR buffer[MaxBufferSize]{};
        DWORD nBytes = 0;
        DWORD nRead = 0;
        void *bio = nullptr;

    private:
        int64_t readWithoutSSL(void *buf, size_t length) noexcept;
        int64_t readWithSSL(void *buffer, size_t length) noexcept;
        int64_t writeWithoutSSL(const void *buf, size_t length) noexcept;
        int64_t writeWithSSL(const void *buffer, size_t length) noexcept;

    public:
#else
        TimerTask::Ptr task = nullptr;
#endif
        bool isClosed = false;
        socket_t socket = -1;
        void *ssl = nullptr;
    };

    struct ServerOption {
        /// 绑定的地址与端口，必填
        IPAddress::Ptr address = nullptr;

        /// 使用线程池大小
        size_t threads = 4;

        /// 此选项用于指示是否使用 SSL，
        /// 开启 SSL 必须设置 sslContext 选项
        bool isSSL = false;
        security::SSLContext::Ptr sslContext = nullptr;

        /// 此选项用于指示是否在 onHandle 结束后自动关闭连接，与 isKeepAlive 互斥
        bool autoClose = true;
        /// 此选项用于指示是否在 onHandle 结束后保留连接一段时间，与 autoClose 互斥且优先级更高
        bool isKeepAlive = false;
        uint32_t keepAlive = 10;

        /// 此函数通常在连接和可读后第一时间被调用。
        /// 返回 true 则表明需要下一步的处理，即将进入 onHandle，
        /// 返回 false 则将不会进入 onHandle，可以进行其他处理
        std::function<bool(sese::net::v2::IOContext *)> beforeHandle = beforeHandler;
        /// 对连接进行正式处理的函数
        std::function<void(sese::net::v2::IOContext *)> onHandle = onHandler;

        static bool beforeHandler(sese::net::v2::IOContext *) noexcept {
            return true;
        }

        static void onHandler(sese::net::v2::IOContext *) noexcept {
            /// 此处一般为业务处理代码，默认实现为空
        }
    };

    class API Server {
    public:
        using Ptr = std::unique_ptr<Server>;
        static Server::Ptr create(const ServerOption &option) noexcept;

    private:
        Server() = default;

    public:
        void loop() noexcept;
        void shutdown() noexcept;

#ifdef WIN32
    private:
        void WindowsWorkerFunction() noexcept;

        SOCKET socket = INVALID_SOCKET;
        HANDLE hIOCP = INVALID_HANDLE_VALUE;
        std::vector<Thread::Ptr> threads;
        void *bioMethod = nullptr;
        std::map<sese::net::v2::IOContext *, sese::TimerTask::Ptr> taskMap;
#elif __linux__
    private:
        void onConnect() noexcept;
        void onRead(socket_t client) noexcept;
        void onClose(socket_t client) noexcept;
        void LinuxWorkerFunction(sese::net::v2::IOContext *ctx) noexcept;

        socket_t socket = -1;
        int epoll = -1;
        epoll_event events[MaxEventSize]{};
        ThreadPool::Ptr threads = nullptr;
        std::map<socket_t, sese::net::v2::IOContext *> contextMap;
#elif __APPLE__
    private:
        void onConnect() noexcept;
        void onRead(socket_t client) noexcept;
        void onClose(socket_t client) noexcept;
        void DarwinWorkerFunction(sese::net::v2::IOContext *ctx) noexcept;

        socket_t socket = -1;
        int kqueue = -1;
        struct kevent events[MaxEventSize] {};
        ThreadPool::Ptr threads = nullptr;
        std::map<socket_t, sese::net::v2::IOContext *> contextMap;
#endif

    private:
        ServerOption option;
        std::atomic_bool isShutdown = false;
        std::mutex mutex;
        Timer::Ptr timer = nullptr;
    };

}// namespace sese::net::v2