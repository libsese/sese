/// \file DNSServer.h
/// \author kaoru
/// \brief DNS 服务器类
/// \version 0.1
/// \date 2023年8月13日
/// \bug 已知 DNS 协议实现存在无法解析二级指针、指针与字面值混合的形式，使用自带的服务端和客户端通信不会存在这个问题，使用其他实现时请谨慎

#pragma once

#include <sese/net/Socket.h>
#include <sese/net/dns/DnsClient.h>
#include <sese/net/dns/DnsSession.h>
#include <sese/thread/Thread.h>

#include <atomic>
#include <map>

namespace sese::net::dns {

/// DNS 配置
struct API DnsConfig {
    IPAddress::Ptr address;
    std::map<std::string, std::string> hostIPv4Map;
    std::map<std::string, std::string> hostIPv6Map;
};

/// DNS 服务器
class API DnsServer {
public:
    using Ptr = std::unique_ptr<DnsServer>;

    /// 使用配置创建 DNS 服务器
    /// \param config 配置
    /// \return 服务器实例
    /// \retval nullptr 创建失败
    static DnsServer::Ptr create(const DnsConfig *config) noexcept;

    ~DnsServer() noexcept;

    /// 启动实例
    void start() noexcept;

    /// 关闭实例
    void shutdown() noexcept;

private:
    DnsServer() = default;

    void loop() noexcept;

    std::atomic_bool isShutdown{false};
    Socket::Ptr socket = nullptr;
    Thread::Ptr thread = nullptr;
    std::map<std::string, std::string> hostIPv4Map;
    std::map<std::string, std::string> hostIPv6Map;
};
} // namespace sese::net::dns