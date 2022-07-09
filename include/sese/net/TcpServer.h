/**
 * @file TcpServer.h
 * @author kaoru
 * @date 2022年5月5日
 * @brief TCP Server 类
 */
#pragma once
#include <sese/net/Socket.h>
#include <sese/thread/ThreadPool.h>
#include <sese/Noncopyable.h>

#ifdef __linux__
// Linux Native Only
#include <sys/epoll.h>
#endif
#ifdef __APPLE__
// Darwin Native Only
#include <sys/event.h>
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

/// @brief IO上下文类
class sese::IOContext {
public:
    friend class sese::TcpServer;

    IOContext() noexcept;
    ~IOContext() noexcept;
    /**
     * @brief 接收指定的字节数
     * @param buffer 缓存
     * @param size 缓存大小, 类 Unix 理想状态下，该值最大可取 64 * 1024
     * @return 实际接收字节数，失败为 -1
     */
    int64_t recv(void *buffer, size_t size) noexcept;
    /**
     * @brief 发送指定字节数
     * @param buffer 缓存
     * @param size 缓存大小, 类 Unix 理想状态下，该值最大可取 64 * 1024
     * @return 实际发送字节数，失败为 -1
     */
    int64_t send(const void *buffer, size_t size) noexcept;
    /**
     * @brief 断开当前连接
     * @param mode 断开的模式
     * @return 操作成功返回 0
     */
    int32_t shutdown(Socket::ShutdownMode mode) const noexcept; // NOLINT
    /**
     * @brief 彻底关闭当前连接
     */
    void close() const noexcept;
    /**
     * @brief 获取客户端的地址信息
     * @return 客户端地址
     */
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
private:
    Socket::Ptr socket{};
#endif
#ifdef __APPLE__
    // Darwin Native Only
private:
    Socket::Ptr socket{};
#endif
};

/// @brief TCP Server 类
class sese::TcpServer : public sese::Noncopyable {
private:
    TcpServer() = default;

public:
    using Ptr = std::unique_ptr<TcpServer>;

    /**
     * @brief 初始化特定平台相关信息
     * @param ipAddress 待绑定地址
     * @param threads 线程池大小
     * @return 初始化成功返回 TcpServer 智能指针，失败返回 nullptr
     */
    static Ptr create(const IPAddress::Ptr &ipAddress, size_t threads = SERVER_DEFAULT_THREADS) noexcept;
    /**
     * @brief 阻塞并在需要时调用相关处理函数
     * @param handler 处理函数
     */
    void loopWith(const std::function<void(IOContext *)> &handler);
    /**
     * @brief 阻塞并关闭服务器并释放资源，直到操作成功
     */
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
private:
    int32_t epollFd = -1;
    epoll_event events[64];
#endif
#ifdef __APPLE__
    // Darwin Native Only
private:
    int32_t kqueueFd = -1;
    struct kevent events[64];
#endif

protected:
    ThreadPool::Ptr threadPool{};
    Socket::Ptr socket{};
    std::atomic_bool isShutdown{true};
};