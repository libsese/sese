/// \file V2Server.h
/// \brief 通用响应式服务器模型
/// \date 2023.02.27
/// \author kaoru
/// \note 先比起原先的服务器提高了可拓展性、可维护性、安全性和性能
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

    /// I/O 上下文
    struct IOContext final {
        /// 从流中读取数据至缓存
        /// \param buffer 缓存
        /// \param length 大小
        /// \return 操作成功字节数
        int64_t read(void *buffer, size_t length) noexcept;
        /// 将缓存写入流中
        /// \param buffer 缓存
        /// \param length 大小
        /// \return 操作成功字节数
        int64_t write(const void *buffer, size_t length) noexcept;
        /// 关闭连接，并标识接下来将进行的收尾工作
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

    /// 服务器选项，用于指定功能的可拓展实体
    struct API ServerOption {
        /// 绑定的地址与端口，必填
        IPAddress::Ptr address = nullptr;

        /// 使用线程池大小
        size_t threads = 4;

        /// 此选项用于指示是否使用 SSL，
        /// 开启 SSL 必须设置 sslContext 选项
        bool isSSL = false;
        /// SSL 上下文
        security::SSLContext::Ptr sslContext = nullptr;

        /// 此选项用于指示是否在 onHandle 结束后自动关闭连接，与 isKeepAlive 互斥
        bool autoClose = true;
        /// 此选项用于指示是否在 onHandle 结束后保留连接一段时间，与 autoClose 互斥且优先级更高
        bool isKeepAlive = false;
        uint32_t keepAlive = 10;

        /// 此函数通常在连接和可读后第一时间被调用。
        /// 返回 true 则表明需要下一步的处理，即将进入 onHandle，
        /// 返回 false 则将不会进入 onHandle，可以进行其他处理
        virtual bool beforeHandle(sese::net::v2::IOContext *) noexcept {
            return true;
        }

        /// 对连接进行正式处理的函数
        virtual void onHandle(sese::net::v2::IOContext *) noexcept {
            /// 此处一般为业务处理代码，默认实现为空
        }
    };

    /// 服务器模型，处理连接和可读事件的调度器
    class API Server {
    public:
        using Ptr = std::unique_ptr<Server>;
        /// 根据已有选项构建一个服务器模型实体
        /// \param option 服务器选项
        /// \return 实例化的服务器模型
        /// \retval nullptr 创建失败返回空指针
        static Server::Ptr create(ServerOption *option) noexcept;

    private:
        Server() = default;

    public:
        /// 循环处理连接，阻塞直至服务器退出
        void loop() noexcept;
        /// 指示服务器退出并清理资源，阻塞至服务器完成退出
        void shutdown() noexcept;

#ifdef WIN32
    private:
        void onConnect() noexcept;
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
        ServerOption *option = nullptr;
        std::atomic_bool isShutdown = false;
        std::mutex mutex;
        Timer::Ptr timer = nullptr;
    };

}// namespace sese::net::v2