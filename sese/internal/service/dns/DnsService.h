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

#include <sese/service/Service.h>
#include <sese/service/dns/Config.h>
#include <sese/net/dns/Resolver.h>
#include <sese/net/dns/DnsPackage.h>
#include <sese/thread/Thread.h>

#include <asio.hpp>

namespace sese::internal::net::service::dns {

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

public:
    DnsService();

    bool bind(const sese::net::IPAddress::Ptr &address);

    void setCallback(const sese::service::dns::Callback &callback);

    void addUpstreamNameServer(const sese::net::IPAddress::Ptr &address);

    bool addUpstreamNameServer(const std::string &ip, uint16_t port = 53);

    void addRecord(const std::string &name, const sese::net::IPAddress::Ptr &address);

    void handle();

    bool startup() override;

    bool shutdown() override;

    int getLastError() override;

    std::string getLastErrorMessage() override;

private:
    sese::service::dns::Callback callback;
};

} // namespace sese::internal::net::service::dns