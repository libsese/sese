/**
 * @file IOCPServer_V1.h
 * @brief 基于 sese-event 的完成端口服务器
 * @author kaoru
 * @version 0.1
 * @date 2023年9月25日
 */

#pragma once

#include <sese/security/SSLContext.h>
#include <sese/service/TimerableService_V2.h>
#include <sese/service/UserBalanceLoader.h>
#include <sese/io/ByteBuilder.h>

#include <set>

namespace sese::iocp {

class Context_V1;
class IOCPServer_V1;
class IOCPService_V1;

/// 基于 sese-event 的完成端口服务器
class IOCPServer_V1 {
public:
    using Context = Context_V1;
    using DeleteContextCallback = std::function<void(Context *data)>;

    IOCPServer_V1();

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
     */
    void postConnect(socket_t sock, const net::IPAddress::Ptr &to, const security::SSLContext::Ptr &cliCtx);
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
    /**
     * 连接前事件回调函数
     * @param ctx 操作上下文
     */
    virtual void onPreConnect(Context *ctx) {}
    /**
     * 连接事件回调函数
     * @param ctx 操作上下文
     */
    virtual void onConnected(Context *ctx) {}
    /**
     * ALPN 协议协商完成回调函数
     * @param ctx 上下文
     * @param in 协商内容
     * @param inLength 协商内容长度
     */
    virtual void onAlpnGet(Context *ctx, const uint8_t *in, uint32_t inLength) {}
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
    void setAddress(const net::IPAddress::Ptr &addr) { balanceLoader.setAddress(addr); }
    /**
     * 设置服务期望线程数量
     * @param threads 线程数量
     */
    void setThreads(size_t threads) { balanceLoader.setThreads(threads); }
    /**
     * 设置服务器监听所使用的 SSL 上下文
     * @param ctx SSL 上下文
     */
    void setServCtx(const security::SSLContext::Ptr &ctx) { IOCPServer_V1::sslCtx = ctx; }
    /**
     * 设置服务器 ALPN 协商内容
     * @param protos 协议协商内容
     */
    void setServProtos(const std::string &protos) { IOCPServer_V1::servProtos = protos; }
    /**
     * 设置服务器操作上下文销毁回调函数
     * @param callback 回调函数
     */
    void setDeleteContextCallback(const DeleteContextCallback &callback) { IOCPServer_V1::deleteContextCallback = callback; }

    /**
     * 获取当前服务器监听 SSL 上下文
     * @return SSL 上下文
     */
    [[nodiscard]] const security::SSLContext::Ptr &getServCtx() const { return IOCPServer_V1::sslCtx; }
    /**
     * 获取当前服务的操作上下文销毁回调函数
     * @return 回调函数
     */
    [[nodiscard]] const DeleteContextCallback &getDeleteContextCallback() const { return IOCPServer_V1::deleteContextCallback; };

public:
    /**
     * 设置连接接入超时时间，此函数仅用户基于 sese-event 的 IOCP 实现
     * @param seconds 连接接入超时时间
     */
    [[maybe_unused]] void setAcceptTimeout(uint32_t seconds) { balanceLoader.setAcceptTimeout(seconds); }
    /**
     * 设置连接分发超时时间，此函数仅用户基于 sese-event 的 IOCP 实现
     * @param seconds 连接分发超时时间
     */
    [[maybe_unused]] void setDispatchTimeout(uint32_t seconds) { balanceLoader.setDispatchTimeout(seconds); }

protected:
    void preConnectCallback(int fd, sese::event::EventLoop *eventLoop, Context *ctx);

    DeleteContextCallback deleteContextCallback = onDeleteContext;
    security::SSLContext::Ptr sslCtx{};
    std::string servProtos{};
    service::UserBalanceLoader balanceLoader;
};

/// 基于 sese-event 的完成端口操作上下文
class Context_V1 final : public io::InputStream, public io::OutputStream, public io::PeekableStream {
    friend class IOCPServer_V1;
    friend class IOCPService_V1;
    IOCPServer_V1 *self{};
    IOCPService_V1 *client{};
    socket_t fd{0};
    void *ssl{};
    bool isConn{false};
    event::BaseEvent *event{};
    service::TimeoutEvent_V2 *timeoutEvent{};
    io::ByteBuilder send{8192, 8192};
    io::ByteBuilder recv{8192, 8192};
    void *data{};

public:
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
     * 从当前连接中读取内容，但不步进
     * @param buffer 缓存
     * @param length 缓存大小
     * @return 实际读取大小
     */
    int64_t peek(void *buffer, size_t length) override;
    /**
     * 在当前连接中步进，不读取内容
     * @param length 步进大小
     * @return 实际步进大小
     */
    int64_t trunc(size_t length) override;
    /**
     * 获取当前上下文连接文件描述符
     * @return 文件描述符
     */
    [[nodiscard]] int32_t getFd() const { return (int32_t) Context_V1::fd; }
    /**
     * 获取当前上下文额外数据
     * @return 额外数据
     */
    [[nodiscard]] void *getData() const { return Context_V1::data; }
    /**
     * 设置当前上下文额外数据
     * @param pData 额外数据
     */
    [[maybe_unused]] void setData(void *pData) { Context_V1::data = pData; }
};

/// 基于 sese-event 的完成端口子服务
class IOCPService_V1 final : public service::TimerableService_V2 {
public:
    /// 子服务操作上下文
    using Context = Context_V1;

    /// 初始化子服务
    /// \param master 主服务器
    explicit IOCPService_V1(IOCPServer_V1 *master);
    /// 析构函数
    ~IOCPService_V1() override;

    /**
     * 向主服务器投递读事件
     * @param ctx 操作上下文
     */
    void postRead(Context *ctx);
    /**
     * 向主服务器投递写事件
     * @param ctx 操作上下文
     */
    void postWrite(Context *ctx);
    /**
     * 向主服务器投递关闭事件
     * @param ctx 操作上下文
     */
    void postClose(Context *ctx);
    /**
     * 子服务器连接接入完成回调函数
     * @param ctx 操作上下文
     */
    static void onAcceptCompleted(Context *ctx);
    /**
     * 子服务读事件触发回调函数
     * @param ctx 操作上下文
     */
    static void onPreRead(Context *ctx);
    /**
     * 子服务读取事件完成触发回调函数
     * @param ctx 操作上下文
     */
    static void onReadCompleted(Context *ctx);
    /**
     * 子服务写入事件完成触发回调函数
     * @param ctx 操作上下文
     */
    static void onWriteCompleted(Context *ctx);
    /**
     * 子服务超时回调函数
     * @param ctx 操作上下文
     */
    static void onTimeout(Context *ctx);
    /**
     * 连接事件回调函数
     * @param ctx 操作上下文
     */
    static void onConnected(Context *ctx);
    /**
     * ALPN 协商完成回调函数
     * @param ctx 操作上下文
     * @param in 协商内容
     * @param inLength 协商内容长度
     */
    static void onAlpnGet(Context *ctx, const uint8_t *in, uint32_t inLength);
    /**
     * ALPN 协商回调函数
     * @param ssl SSL 上下文
     * @param out 对端期望内容
     * @param outLength 对端期望内容长度
     * @param in 响应内容
     * @param inLength 响应内容长度
     * @param service 所属子服务
     * @return ALPN 状态码
     */
    static int alpnCallbackFunction(
            void *ssl,
            const uint8_t **out, uint8_t *outLength,
            const uint8_t *in, uint32_t inLength,
            IOCPService_V1 *service
    );

private:
    void onAccept(int fd) override;
    void onRead(event::BaseEvent *event) override;
    void onWrite(event::BaseEvent *event) override;
    void onClose(event::BaseEvent *event) override;
    void onTimeout(service::TimeoutEvent_V2 *event) override;

    event::BaseEvent *createEventEx(int fd, unsigned int events, void *data);
    void freeEventEx(sese::event::BaseEvent *event);

    static int64_t read(int fd, void *buffer, size_t len, void *ssl);
    static int64_t write(int fd, const void *buffer, size_t len, void *ssl);

    IOCPServer_V1 *master{};
    std::set<event::BaseEvent *> eventSet;
};

} // namespace sese::iocp