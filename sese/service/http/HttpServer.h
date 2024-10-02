/// @file HttpServer.h
/// @brief Http 服务器
/// @author kaoru
/// @date 2024年05月1日

#pragma once

#include <sese/service/http/HttpService.h>
#include <sese/net/http/Controller.h>

namespace sese::service::http {

/// Http 服务器
/// @note 调用优先级 Filter > Mount Point(Filter) > Controller = Servlet，互相独立不可转换
class HttpServer final {
public:
    /// 注册控制器
    /// @tparam CTL 控制器类型
    /// @tparam ARGS 实例化参数类型
    /// @param args 实例化参数
    template<class CTL, class... ARGS>
    void regController(ARGS &&...args);

    /// 注册文件系统挂载点
    /// @param uri_prefix URI 前缀
    /// @param local 本地路径
    void regMountPoint(const std::string &uri_prefix, const std::string &local);

    /// 注册过滤器
    /// \param uri_prefix URI 前缀
    /// \param callback 回调函数，函数返回 true 需要下一步处理即继续判断之后的 Mount Point 以及 Controller 等，反之则拦截当前请求并直接响应
    void regFilter(const std::string &uri_prefix, const HttpService::FilterCallback &callback);

    /// 注册 Http 应用
    /// @param servlet Http 应用
    void regServlet(const net::http::Servlet &servlet);

    /// 此方法用于注册一个后置 Filter，它将在其他所有 Servlet、控制器和挂载点处理异常之后执行。
    /// 如果需要对响应进行最终修改或处理（自定义404等页面），可以使用此功能。
    /// 返回值表示是否拦截做了处理，拦截后响应类型将会变为 Controller，并接受相关处理。
    /// \param tail_filter 待注册的后置 Http Filter，用于处理请求
    void regTailFilter(const HttpService::FilterCallback &tail_filter);

    /// 设置 Keepalive
    /// @param seconds Keepalive 时间，最小值为 5
    void setKeepalive(uint32_t seconds);

    /// 注册 Http 服务
    /// @param address 监听地址
    /// @param context SSL 服务上下文，为空则不启用 SSL
    void regService(const net::IPAddress::Ptr &address, std::unique_ptr<security::SSLContext> context);

    /// 设置服务器名称
    /// @param name 服务器名称
    void setName(const std::string &name);

    /// 设置连接回调函数，函数会在连接对象被创建前调用，SSL 连接尚未握手
    /// @param callback 连接回调函数，函数返回 true 将继续正常处理，反之则直接丢弃该连接
    void setConnectionCallback(const HttpService::ConnectionCallback &callback);

    /// 启动服务
    /// @return 结果
    bool startup()const;

    /// 终止服务
    /// @return 结果
    bool shutdown()const;

private:
    std::string name;
    uint32_t keepalive = 5;
    std::vector<HttpService::Ptr> services;
    std::vector<net::http::Controller::Ptr> controllers;
    HttpService::MountPointMap mount_points;
    HttpService::ServletMap servlets;
    HttpService::FilterMap filters;
    HttpService::FilterCallback tail_filter;
    HttpService::ConnectionCallback connection_callback;
};

template<class CTL, class... ARGS>
void HttpServer::regController(ARGS &&...args) {
    auto controller = std::make_shared<CTL>(std::forward<ARGS>(args)...);
    controller->init();
    for (auto &&servlet: *controller) {
        servlets.emplace(servlet.getUri(), servlet);
    }
    controllers.push_back(controller); // GCOVR_EXCL_LINE
}

} // namespace sese::service::http::v3