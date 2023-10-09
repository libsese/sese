/// @file HttpService_V1.h
/// @brief HTTP 服务
/// @version 0.1
/// @author kaoru
/// @date 2023年8月4日

#pragma once

#include <sese/security/SecuritySocket.h>
#include <sese/Config.h>
#include <sese/security/SSLContext.h>
#include <sese/net/http/ControllerGroup.h>

#ifdef _WIN32
#pragma warning(disable : 4275)
#endif

namespace sese::service::v1 {

/// HTTP 服务配置
struct API HttpConfig {
    HttpConfig() noexcept;

    /// 为当前配置添加通用控制器
    /// \param path 注册路径
    /// \param controller2 控制器
    void setController(const std::string &path, const net::http::Controller &controller2) noexcept;

    /// 为当前配置添加控制器组
    /// \param group 控制器组
    void setController(const net::http::ControllerGroup &group) noexcept;

    /// 响应中自动添加 Server 字段值
    std::string servName = HTTPD_NAME;
    /// SSL 上下文，为空则不开启 HTTPS
    security::SSLContext::Ptr servCtx = nullptr;
    /// 连接保留时间，为 0 则每次响应都会关闭连接
    uint64_t keepalive = 0;
    /// 服务器工作目录，如果为空则不开启文件下载功能
    std::string workDir;

    /// 默认控制器，不存在控制器或文件时被调用
    net::http::Controller otherController;
    /// 控制器映射，一般不需要手动更改
    std::map<std::string, net::http::Controller> controllerMap;
};

/// @brief HTTP 服务，支持长连接、文件下载、断点续传等
// class API HttpService final : public sese::event::EventLoop {
class API HttpService : public sese::service::TimerableService_V1 {
public:
    /// 通过配置构造 Http 服务
    /// \param config
    explicit HttpService(HttpConfig *config) noexcept;

    /// 析构
    ~HttpService() noexcept override;

    /// 连接接入时触发
    /// \param fd 套接字文件描述符
    void onAccept(int fd) override;

    /// 连接可读时触发
    /// \param event 事件
    void onRead(event::BaseEvent *event) override;

    /// 连接可写时触发
    /// \warning 注意 WSAEventSelect 后端无法通过直接手动触发这个事件
    /// \param event 事件
    void onWrite(event::BaseEvent *event) override;

    /// 连接对端关闭时触发
    /// \param event 事件
    void onClose(event::BaseEvent *event) override;

protected:
    /// 新建并保存事件
    /// \param fd 套接字文件描述符
    /// \param events 事件组合
    /// \param data 额外数据（HttpConnection *）
    /// \return 事件指针
    event::BaseEvent *createEventEx(int fd, unsigned int events, void *data) noexcept;

    /// 释放并删除保存的事件
    /// \param event 事件指针
    void freeEventEx(event::BaseEvent *event) noexcept;

    /// 普通控制器处理
    /// \param conn Http 连接
    virtual void onHandle(net::http::HttpConnection *conn) noexcept;

    /// 协议升级处理
    /// \param conn Http 连接
    virtual void onHandleUpgrade(net::http::HttpConnection *conn) noexcept;

    /// 文件处理
    /// \param conn Http 连接
    /// \param path 文件绝对路径
    void onHandleFile(net::http::HttpConnection *conn, const std::string &path) noexcept;

    /// 连接超时时处理
    /// \param timeoutEvent 超时事件
    void onTimeout(TimeoutEvent_V1 *timeoutEvent) override;

    /// 发送控制器数据处理
    /// \param event 事件
    void onControllerWrite(event::BaseEvent *event) noexcept;

    /// 发送文件数据处理
    /// \param event 事件
    void onFileWrite(event::BaseEvent *event) noexcept;

    /// 在接收或发送过程中的以外故障导致的关闭时间
    /// \param event 事件
    virtual void onProcClose(event::BaseEvent *event) noexcept;

    /// 从连接中读取内容，自动选择是否为 ssl 连接
    /// \param fd 套接字文件描述符
    /// \param buffer 缓存
    /// \param len 缓存大小
    /// \param ssl ssl
    /// \return 读取字节总数
    static int64_t read(int fd, void *buffer, size_t len, void *ssl) noexcept;

    /// 向连接中写入内容，自动选择是否为 ssl 连接
    /// \param fd 套接字文件描述符
    /// \param buffer 缓存
    /// \param len 缓存大小
    /// \param ssl ssl
    /// \return 写入字节总数
    static int64_t write(int fd, const void *buffer, size_t len, void *ssl) noexcept;

    HttpConfig *config;
    std::map<int, event::BaseEvent *> eventMap;
};
} // namespace sese::service::v1