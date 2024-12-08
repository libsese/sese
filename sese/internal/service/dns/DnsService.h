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
/// @brief DNS service
/// @author kaoru
/// @date October 30, 2024

#pragma once

#include <sese/service/Service.h>
#include <sese/service/dns/Config.h>
#include <sese/net/dns/Resolver.h>
#include <sese/net/dns/DnsPackage.h>
#include <sese/thread/Thread.h>

#include <asio.hpp>

namespace sese::internal::net::service::dns {

/// @brief DNS service
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

    DnsService();

    /// @brief Bind address
    /// @param address The address
    /// @return Whether binding was successful
    bool bind(const sese::net::IPAddress::Ptr &address);

    /// @brief Set callback, the callback function precedes domain judgment logic, similar to a filter. The return value indicates whether further processing is needed
    /// @param callback The callback function
    void setCallback(const sese::service::dns::Callback &callback);

    /// @brief Add upstream DNS server
    /// @param address The address
    void addUpstreamNameServer(const sese::net::IPAddress::Ptr &address);

    /// @brief Add upstream DNS server
    /// @param ip IP address
    /// @param port Port number
    /// @return Whether the addition was successful, which depends on the format of the provided IP address
    bool addUpstreamNameServer(const std::string &ip, uint16_t port = 53);

    /// @brief Add record
    /// @param name Domain name
    /// @param address Address
    void addRecord(const std::string &name, const sese::net::IPAddress::Ptr &address);

    /// @brief Start service
    /// @return Whether it was successful
    bool startup() override;

    /// @brief Stop service
    /// @return Whether it was successful
    bool shutdown() override;

    /// @brief Get error code
    /// @return Error code
    int getLastError() override;

    /// @brief Get error message
    /// @return Error message
    std::string getLastErrorMessage() override;

private:
    sese::service::dns::Callback callback;
};

} // namespace sese::internal::net::service::dns