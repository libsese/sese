// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// @file DnsService.h
/// @brief DNS 服务
/// @author kaoru
/// @date 2024年10月30日

#pragma once

#include <sese/service/Service.h>
#include <sese/service/dns/Config.h>
#include <sese/net/dns/Resolver.h>
#include <sese/net/dns/DnsPackage.h>
#include <sese/thread/Thread.h>

#include <asio.hpp>

namespace sese::internal::net::service::dns {

/// @brief DNS 服务
class DnsService final : public sese::service::Service {
    asio::io_service io_service;
    asio::ip::udp::socket socket;
    asio::error_code error;
    std::array<uint8_t, 512> buffer;
    sese::net::dns::Resolver resolver;
    Thread::Ptr thread;
    std::atomic_bool running;

    std::map<std::string, sese::net::IPv4Address::Ptr> v4map;
    std::map<std::string, sese::net::IPv6Address::Ptr> v6map;

    void handleBySelf(
        std::vector<sese::net::dns::DnsPackage::Question> &questions,
        sese::net::dns::DnsPackage::Ptr &send_package
    );

    void handleByUpstream(
        std::vector<sese::net::dns::DnsPackage::Question> &questions,
        sese::net::dns::DnsPackage::Ptr &send_package
    );

    void handle();

public:

    /// @brief 构造函数
    DnsService();

    /// @brief 绑定地址
    /// @param address 地址
    /// @return 是否绑定成功
    bool bind(const sese::net::IPAddress::Ptr &address);

    /// @brief 设置回调，回调函数先于域名判断逻辑，类似于过滤器，返回值代表是否需要下一步处理
    /// @param callback 回调
    void setCallback(const sese::service::dns::Callback &callback);

    /// @brief 添加上游域名服务器
    /// @param address 地址
    void addUpstreamNameServer(const sese::net::IPAddress::Ptr &address);

    /// @brief 添加上游域名服务器
    /// @param ip IP
    /// @param port 端口
    /// @return 是否添加成功，这取决于填写的IP地址格式
    bool addUpstreamNameServer(const std::string &ip, uint16_t port = 53);

    /// @brief 添加记录
    /// @param name 域名
    /// @param address 地址
    void addRecord(const std::string &name, const sese::net::IPAddress::Ptr &address);

    /// @brief 启动服务
    /// @return 是否成功
    bool startup() override;

    /// @brief 停止服务
    /// @return 是否成功
    bool shutdown() override;

    /// @brief 获取错误码
    /// @return 错误码
    int getLastError() override;

    /// @brief 获取错误信息
    /// @return 错误信息
    std::string getLastErrorMessage() override;

private:
    sese::service::dns::Callback callback;
};

} // namespace sese::internal::net::service::dns