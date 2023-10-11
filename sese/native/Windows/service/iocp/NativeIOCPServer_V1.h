/**
 * @file NativeIOCPServer_V1.h
 * @brief Windows 原生 IOCP 服务器
 * @author kaoru
 * @version 0.1
 * @date 2023年9月25日
 */

#pragma once

#include <sese/net/Address.h>
#include <sese/security/SSLContext.h>
#include <sese/service/iocp/IOBuf.h>
#include <sese/thread/Thread.h>
#include <sese/io/ByteBuilder.h>
#include <sese/util/TimeWheel.h>

#include <atomic>
#include <set>
#include <mutex>

namespace sese::_windows::iocp {

class NativeIOCPServer_V1;
struct OverlappedWrapper;

/// 原生 IOCP 操作上下文
class NativeContext_V1 final : public io::InputStream, public io::OutputStream {
    friend class NativeIOCPServer_V1;
    using IOBuf = sese::iocp::IOBuf;
    using Node = sese::iocp::IOBufNode;

    enum class Type {
        Read,
        Write,
        Connect,
        Close
    };

    OverlappedWrapper *pWrapper{};
    WSABUF wsabufRead{};
    WSABUF wsabufWrite{};
    Type type{Type::Read};
    SOCKET fd{INVALID_SOCKET};
    NativeIOCPServer_V1 *self{};
    TimeoutEvent *timeoutEvent{};
    void *ssl{};
    void *bio{};
    Node *readNode{};
    IOBuf recv{};
    io::ByteBuilder send{IOCP_WSABUF_SIZE, IOCP_WSABUF_SIZE};
    void *data{};

public:
    /**
     * 上下文初始化
     * @param pWrapper Overlapped 包装器
     */
    explicit NativeContext_V1(OverlappedWrapper *pWrapper);
    /// 析构函数
    ~NativeContext_V1() override;
    /**
     * 从当前连接中读取内容
     * @param buffer 缓存
     * @param length 缓存大小
     * @return 实际读取大小
     */
    int64_t read(void *buffer, size_t length) override;
    /**
     * 向当前连接中写入内容
     * @param buffer 缓存
     * @param length 缓存大小
     * @return 实际写入大小
     */
    int64_t write(const void *buffer, size_t length) override;

    /**
     * 获取当前上下文连接文件描述符
     * @return 文件描述符
     */
    [[nodiscard]] int32_t getFd() const { return (int32_t) NativeContext_V1::fd; }
    /**
     * 获取当前上下文额外数据
     * @return 额外数据
     */
    [[nodiscard]] void *getData() const { return NativeContext_V1::data; }
    /**
     * 设置当前上下文额外数据
     * @param pData 额外数据
     */
    void setData(void *pData) { NativeContext_V1::data = pData; }
};

/// Overlapped 包装器
struct OverlappedWrapper final {
    OVERLAPPED overlapped{};
    NativeContext_V1 ctx;

    OverlappedWrapper();
};

/// Windows 原生 IOCP 服务器
class NativeIOCPServer_V1 {
public:
    using Context = NativeContext_V1;
    using DeleteContextCallback = std::function<void(Context *data)>;

    /// 析构函数
    virtual ~NativeIOCPServer_V1() = default;

    /**
     * 初始化并启动服务器
     * @return 初始化结果
     */
    bool init();
    /**
     * 终止工作线程、释放系统资源并结束服务器
     */
    void shutdown();
    /**
     * 投递读事件
     * @param ctx 操作上下文
     */
    void postRead(Context *ctx);
    /**
     * 投递写事件
     * @param ctx 操作上下文
     */
    void postWrite(Context *ctx);
    /**
     * 投递关闭事件
     * @param ctx 操作上下文
     */
    void postClose(Context *ctx);
    /**
     * 投递连接事件
     * @param sock 套接字
     * @param to 连接地址
     * @return 操作上下文
     */
    Context *postConnect(socket_t sock, const net::IPAddress::Ptr &to, const security::SSLContext::Ptr &cliCtx);
    /**
     * 设置超时事件
     * @param ctx 操作上下文
     * @param seconds 超时时间
     */
    void setTimeout(Context *ctx, int64_t seconds);
    /**
     * 取消超时事件
     * @param ctx 操作上下文
     */
    void cancelTimeout(Context *ctx);
    /**
     * 默认的上下文释放回调函数
     */
    static void onDeleteContext(Context *) {}
    /**
     * 连接握手完成回调函数
     * @param ctx 操作上下文
     */
    virtual void onAcceptCompleted(Context *ctx) {}
    /**
     * 读取事件触发回调函数
     * @param ctx 操作上下文
     */
    virtual void onPreRead(Context *ctx) {}
    /**
     * 读取事件完成回调函数
     * @param ctx 操作上下文
     */
    virtual void onReadCompleted(Context *ctx) {}
    /**
     * 写入事件完成回调函数
     * @param ctx 操作上下文
     */
    virtual void onWriteCompleted(Context *ctx) {}
    /**
     * 超时事件回调函数
     * @param ctx 操作上下文
     */
    virtual void onTimeout(Context *ctx) {}

    virtual void onConnected(Context *ctx) {};
    /**
     * ALPN 协议协商完成回调函数
     * @param ctx 上下文
     * @param in 协商内容
     * @param inLength 协商内容长度
     */
    virtual void onAlpnGet(Context *ctx, const uint8_t *in, uint32_t inLength){};
    /**
     * ALPN 协商回调函数
     * @param out 对端期望内容
     * @param outLength 对端期望内容长度
     * @param in 响应内容
     * @param inLength 响应内容长度
     * @return ALPN 状态码
     */
    int onAlpnSelect(
            const uint8_t **out, uint8_t *outLength,
            const uint8_t *in, uint32_t inLength
    );

public:
    /**
     * 设置当前服务绑定的 IP 地址，设置此选项服务器将会自动监听对应地址的端口
     * @param addr 目标 IP
     */
    void setAddress(const net::IPAddress::Ptr &addr) { NativeIOCPServer_V1::address = addr; }
    /**
     * 设置服务期望线程数量
     * @param numberOfThreads 线程数量
     */
    void setThreads(size_t numberOfThreads) { NativeIOCPServer_V1::threads = numberOfThreads; }
    /**
     * 设置服务器监听所使用的 SSL 上下文
     * @param ctx SSL 上下文
     */
    void setServCtx(const security::SSLContext::Ptr &ctx) { NativeIOCPServer_V1::sslCtx = ctx; }
    /**
     * 设置服务器 ALPN 协商内容
     * @param protos 协议协商内容
     */
    void setServProtos(const std::string &protos) { NativeIOCPServer_V1::servProtos = protos; }
    /**
     * 设置服务器操作上下文销毁回调函数
     * @param callback 回调函数
     */
    void setDeleteContextCallback(const DeleteContextCallback &callback) { NativeIOCPServer_V1::deleteContextCallback = callback; }

    /**
     * 获取当前服务器监听 SSL 上下文
     * @return SSL 上下文
     */
    [[nodiscard]] const security::SSLContext::Ptr &getServCtx() const { return NativeIOCPServer_V1::sslCtx; }
    /**
     * 获取当前服务的操作上下文销毁回调函数
     * @return 回调函数
     */
    [[nodiscard]] const DeleteContextCallback &getDeleteContextCallback() const { return NativeIOCPServer_V1::deleteContextCallback; };

protected:
    void acceptThreadProc();
    void eventThreadProc();
    static int alpnCallbackFunction(
            void *ssl,
            const uint8_t **out, uint8_t *outLength,
            const uint8_t *in, uint32_t inLength,
            NativeIOCPServer_V1 *server
    );

    void *connectEx{};
    bool initConnectEx();

    static long bioCtrl(void *bio, int cmd, long num, void *ptr);
    static int bioWrite(void *bio, const char *in, int length);
    static int bioRead(void *bio, char *out, int length);

    std::atomic_bool isShutdown{false};
    HANDLE iocpFd{INVALID_HANDLE_VALUE};
    SOCKET listenFd{INVALID_SOCKET};
    net::IPAddress::Ptr address{};
    Thread::Ptr acceptThread{};

    TimeWheel wheel{};
    std::set<OverlappedWrapper *> wrapperSet{};
    std::mutex wrapperSetMutex{};

    size_t threads{};
    std::vector<Thread::Ptr> eventThreadGroup{};
    DeleteContextCallback deleteContextCallback = onDeleteContext;
    security::SSLContext::Ptr sslCtx{};
    void *bioMethod{};
    std::string servProtos{};
};

} // namespace sese::_windows::iocp