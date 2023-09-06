/// \file V2Server.h
/// \brief 通用响应式服务器模型
/// \date 2023.04.29
/// \author kaoru
/// \note 先比起原先的服务器提高了可拓展性、可维护性、安全性和性能

#pragma once

#include "sese/net/Socket.h"
#include "sese/security/SSLContext.h"
#include "sese/thread/ThreadPool.h"
#include "sese/util/Noncopyable.h"

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

    class API [[deprecated]] WindowsService {
    public:
        virtual ~WindowsService() noexcept;

        /// 初始化当前服务
        /// \return 初始化结果
        virtual bool init() noexcept;

        void start() noexcept;

        void shutdown() noexcept;

    protected:
        void loop() noexcept;

        void *handshake(SOCKET clientSocket) noexcept;

        void connect(WindowsServiceIOContext ctx) noexcept;

        void handle(WindowsServiceIOContext ctx) noexcept;

        void closing(WindowsServiceIOContext ctx) noexcept;

    protected:
        bool initStatus = false;
        std::atomic_bool exitStatus{false};
        DWORD eventNum = 0;
        SOCKET socketSet[MaxEventSize]{};
        HANDLE hEventSet[MaxEventSize]{};
        PVOID sslSet[MaxEventSize]{};

        Thread::Ptr mainThread{nullptr};
        ThreadPool::Ptr threadPool{nullptr};

    public:
        /// 当连接首次接入时触发
        virtual void onConnect(sese::net::v2::WindowsServiceIOContext &) noexcept {
        }

        /// 对连接进行正式处理的函数
        virtual void onHandle(sese::net::v2::WindowsServiceIOContext &) noexcept {
            /// 此处一般为业务处理代码，默认实现为空
        }

        /// 当对端关闭或者主动 shutdown 时触发，此时连接可能已断开
        virtual void onClosing(sese::net::v2::WindowsServiceIOContext &) noexcept {
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

    class API [[deprecated]] WindowsServiceIOContext {
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

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    using IOContext = WindowsServiceIOContext;
    using Server = WindowsService;
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif

#ifdef SESE_PLATFORM_LINUX

    class LinuxServiceIOContext;

    class API [[deprecated]] LinuxService : public Noncopyable {
    public:
        virtual ~LinuxService() noexcept;

        /// 初始化当前服务
        /// \return 初始化结果
        virtual bool init() noexcept;

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
        std::atomic_bool exitStatus{false};
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

    class API [[deprecated]] LinuxServiceIOContext {
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

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    using IOContext = LinuxServiceIOContext;
    using Server = LinuxService;
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#endif

#ifdef SESE_PLATFORM_APPLE

    class DarwinServiceIOContext;

    class API [[deprecated]] DarwinService {
    public:
        virtual ~DarwinService() noexcept;

        virtual bool init() noexcept;

        void start() noexcept;

        void shutdown() noexcept;

    protected:
        void loop() noexcept;

        void *handshake(socket_t client) noexcept;

        void connect(DarwinServiceIOContext ctx) noexcept;

        void handle(DarwinServiceIOContext ctx) noexcept;

        void closing(DarwinServiceIOContext ctx) noexcept;

    protected:
        bool initStatus = false;
        std::atomic_bool exitStatus{false};
        int kqueue = -1;
        socket_t socket = -1;
        struct kevent eventSet[MaxEventSize] {};

        Thread::Ptr mainThread{nullptr};
        ThreadPool::Ptr threadPool{nullptr};
        public:
        /// 当连接首次接入时触发
        virtual void onConnect(sese::net::v2::DarwinServiceIOContext &) noexcept {
        }

        /// 对连接进行正式处理的函数
        virtual void onHandle(sese::net::v2::DarwinServiceIOContext &) noexcept {
            /// 此处一般为业务处理代码，默认实现为空
        }

        /// 当对端关闭或者主动 shutdown 时触发，此时连接可能已断开
        virtual void onClosing(sese::net::v2::DarwinServiceIOContext &) noexcept {
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

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    using IOContext = DarwinServiceIOContext;
    using Server = DarwinService;
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif

}// namespace sese::net::v2