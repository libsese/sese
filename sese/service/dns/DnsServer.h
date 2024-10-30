/// @file DnsServer.h
/// @brief DNS 服务器
/// @author kaoru
/// @date 2024年10月30日

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

#pragma once

#include "Config.h"

#include <sese/service/Service.h>

namespace sese::service::dns {

/// @brief DNS 服务器
class DnsServer final {
    Service::Ptr service;

public:
    /// @brief 构造函数
    DnsServer();

    /// @brief 绑定地址
    /// @param address 地址
    /// @return 是否绑定成功
    bool bind(const net::IPAddress::Ptr &address);

    /// @brief 设置回调
    /// @param callback 回调
    void setCallback(const Callback &callback);

    /// @brief 添加上游服务器
    /// @param address 地址
    void addUpstreamNameServer(const net::IPAddress::Ptr &address);

    /// @brief 添加上游服务器
    /// @param ip 地址
    /// @param port 端口
    /// @return 是否添加成功
    bool addUpstreamNameServer(const std::string &ip, uint16_t port = 53);

    /// @brief 添加记录
    /// @param name 域名
    /// @param address 地址
    void addRecord(const std::string &name, const net::IPAddress::Ptr &address);

    /// @brief 启动
    /// @return 是否启动成功
    bool startup();

    /// @brief 停止
    /// @return 是否停止成功
    bool shutdown();

    /// @brief 获取错误码
    /// @return 错误码
    int getLastError();

    /// @brief 获取错误信息
    /// @return 错误信息
    std::string getLastErrorMessage();
};

} // namespace sese::service::dns
