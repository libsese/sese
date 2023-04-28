/// \file V2Server.h
/// \brief 通用响应式服务器模型
/// \date 2023.04.15
/// \author kaoru
/// \note 先比起原先的服务器提高了可拓展性、可维护性、安全性和性能

#pragma once

#include "sese/net/Socket.h"
#include "sese/security/SSLContext.h"
#include "sese/thread/ThreadPool.h"

#if defined(SESE_PLATFORM_WINDOWS)
#define MaxEventSize 64
#elif defined(SESE_PLATFORM_LINUX)
#define MaxEventSize 64
#include <sys/epoll.h>
#elif defined(SESE_PLATFORM_APPLE)
#define MaxEventSize 64
#include <sys/event.h>
#endif

namespace sese::net::v2 {

#ifdef SESE_PLATFORM_WINDOWS

    class WindowsServiceIOContext;

    class API WindowsService {
    public:
        using Ptr = std::unique_ptr<WindowsService>;

        static WindowsService::Ptr create(ServerOption *opt) noexcept;

        void start() noexcept;

        void shutdown() noexcept;

    protected:
        void loop() noexcept;

        void *handshake(SOCKET clientSocket) noexcept;

        void connect(WindowsServiceIOContext ctx) noexcept;

        void handle(WindowsServiceIOContext ctx) noexcept;

        void closing(WindowsServiceIOContext ctx) noexcept;

    protected:
        bool exit = false;
        DWORD eventNum = 0;
        SOCKET socketSet[MaxEventSize]{};
        HANDLE hEventSet[MaxEventSize]{};
        PVOID sslSet[MaxEventSize]{};

        ServerOption *option{nullptr};
        Thread::Ptr mainThread{nullptr};
        ThreadPool::Ptr threadPool{nullptr};
    };

    class API WindowsServiceIOContext {
        friend class WindowsService;

    public:
        WindowsServiceIOContext(socket_t socket, HANDLE event, void *ssl) noexcept;

        int64_t peek(void *buf, size_t len) noexcept;

        int64_t read(void *buf, size_t len) noexcept;

        int64_t write(const void *buf, size_t len) noexcept;

        void close() noexcept;

        [[nodiscard]] socket_t getIdent() const noexcept { return socket; }

    protected:
        socket_t socket;
        void *ssl = nullptr;
        bool isClosing = false;
        HANDLE event;
    };

    using IOContext = WindowsServiceIOContext;
    using Server = WindowsService;

#endif

#ifdef SESE_PLATFORM_LINUX

    class LinuxServiceIOContext;

    class API LinuxService {
    public:
        using Ptr = std::unique_ptr<LinuxService>;

        virtual ~LinuxService() noexcept;

        bool init() noexcept;

        void start() noexcept;

        void shutdown() noexcept;

    protected:
        void loop() noexcept;

        void *handshake(socket_t client) noexcept;

        void connect(LinuxServiceIOContext ctx) noexcept;

        void handle(LinuxServiceIOContext ctx) noexcept;

        void closing(LinuxServiceIOContext ctx) noexcept;

    protected:
        bool initStatus = false;
        bool exitStatus = false;
        int epoll = -1;
        socket_t socket = -1;
        epoll_event eventSet[MaxEventSize]{};

        Thread::Ptr mainThread{nullptr};
        ThreadPool::Ptr threadPool{nullptr};

    public:
        /// 当连接首次接入时触发
        virtual void onConnect(sese::net::v2::LinuxServiceIOContext &) noexcept {
        }

        /// 对连接进行正式处理的函数
        virtual void onHandle(sese::net::v2::LinuxServiceIOContext &) noexcept {
            /// 此处一般为业务处理代码，默认实现为空
        }

        /// 当对端关闭或者主动 shutdown 时触发，此时连接可能已断开
        virtual void onClosing(sese::net::v2::LinuxServiceIOContext &) noexcept {
        }

        /// 设置服务绑定的地址，默认为 localhost:8080
        /// \param addr IP 地址
        void setBindAddress(const IPAddress::Ptr &addr) noexcept { address = addr; }

        /// 设置服务创建的线程池大小，默认为 4
        /// \param size 线程池大小
        void setThreadPoolSize(size_t size) noexcept { threads = size; }

        /// 设置SSL上下文，使用此选项将启用SSL，默认为 nullptr
        /// \param ctx 上下文
        void setSSLContext(const security::SSLContext::Ptr &ctx) noexcept { sslContext = ctx; }
    protected:
        size_t threads = 4;
        IPAddress::Ptr address = nullptr;
        security::SSLContext::Ptr sslContext = nullptr;
    };

    class API LinuxServiceIOContext {
        friend class LinuxService;

    public:
        LinuxServiceIOContext(socket_t socket, void *ssl) noexcept;

        int64_t peek(void *buf, size_t len) noexcept;

        int64_t read(void *buf, size_t len) noexcept;

        int64_t write(const void *buf, size_t len) noexcept;

        void close() noexcept;

        [[nodiscard]] socket_t getIdent() const noexcept { return socket; }

    protected:
        socket_t socket;
        void *ssl = nullptr;
        bool isClosing = false;
    };

    using IOContext = LinuxServiceIOContext;
    using Server = LinuxService;

#endif

#ifdef SESE_PLATFORM_APPLE

    class DarwinServiceIOContext;

    class API DarwinService {
    public:
        using Ptr = std::unique_ptr<DarwinService>;

        static DarwinService::Ptr create(ServerOption *opt) noexcept;

        void start() noexcept;

        void shutdown() noexcept;

    protected:
        void loop() noexcept;

        void *handshake(socket_t client) noexcept;

        void connect(DarwinServiceIOContext ctx) noexcept;

        void handle(DarwinServiceIOContext ctx) noexcept;

        void closing(DarwinServiceIOContext ctx) noexcept;

    protected:
        bool exit = false;
        int kqueue = -1;
        socket_t socket = -1;
        struct kevent eventSet[MaxEventSize] {};

        ServerOption *option{nullptr};
        Thread::Ptr mainThread{nullptr};
        ThreadPool::Ptr threadPool{nullptr};
    };

    class API DarwinServiceIOContext {
        friend class DarwinService;

    public:
        DarwinServiceIOContext(socket_t socket, void *ssl) noexcept;

        int64_t peek(void *buf, size_t len) noexcept;

        int64_t read(void *buf, size_t len) noexcept;

        int64_t write(const void *buf, size_t len) noexcept;

        void close() noexcept;

        [[nodiscard]] socket_t getIdent() const noexcept { return socket; }

    protected:
        socket_t socket;
        void *ssl = nullptr;
        bool isClosing = false;
    };

    using IOContext = DarwinServiceIOContext;
    using Server = DarwinService;

#endif

}// namespace sese::net::v2