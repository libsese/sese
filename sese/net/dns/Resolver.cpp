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

#include "Resolver.h"
#include "DnsPackage.h"
#include "Config.h"

#include <sese/internal/net/AsioIPConvert.h>

#include <asio.hpp>

using sese::net::dns::DnsPackage;
using sese::net::dns::Resolver;

Resolver::Resolver() : generator(device()) {
}

bool Resolver::addNameServer(const std::string &ip, uint16_t port) {
    auto name_server = IPAddress::create(ip.c_str(), port);
    if (!name_server) {
        return false;
    }
    name_servers.push_back(name_server);
    return true;
}

void Resolver::addNameServer(const IPAddress::Ptr &ip_address) {
    name_servers.push_back(ip_address);
}

std::vector<sese::net::IPAddress::Ptr> Resolver::resolve(const std::string &hostname, uint16_t type) {
    for (auto name_server: name_servers) {
        auto addresses = resolve(name_server, hostname, type);
        if (!addresses.empty()) {
            return addresses;
        }
    }
    return {};
}

std::vector<sese::net::IPAddress::Ptr> Resolver::resolve(const IPAddress::Ptr &name_server, const std::string &hostname, uint16_t type) {
    std::vector<IPAddress::Ptr> result;
    DnsPackage::Flags flags;

    uint8_t buffer[512];
    size_t length = sizeof(buffer);

    DnsPackage::Index index;
    auto send_package = DnsPackage::new_();
    send_package->getQuestions().emplace_back(hostname, type, CLASS_IN);
    send_package->setId(std::uniform_int_distribution<uint16_t>(0, 255)(generator));
    send_package->setFlags(flags.encode());

    if (!send_package->encode(buffer, length, index)) {
        return result;
    }

    auto addr = internal::net::convert(name_server);
    auto endpoint = asio::ip::udp::endpoint(addr, name_server->getPort());
    auto io_context = asio::io_context();
    auto socket = asio::ip::udp::socket(io_context);
    asio::error_code ec;
    ec = socket.open(endpoint.protocol(), ec);
    if (ec) {
        return result;
    }
    socket.send_to(asio::buffer(buffer, length), endpoint);
    length = socket.receive_from(asio::buffer(buffer), endpoint, 0, ec);
    socket.close();
    if (ec) {
        return result;
    }

    auto package = DnsPackage::decode(buffer, length);
    if (!package ||
        package->getId() != send_package->getId()) {
        return result;
    }
    for (auto &&answer: package->getAnswers()) {
        if (answer.type != type ||
            answer.data_length == 0 ||
            answer.class_ != CLASS_IN) {
            continue;
        }
        if (answer.type == TYPE_A && answer.data_length == 4) {
            sockaddr_in sockaddr{};
            sockaddr.sin_family = AF_INET;
            memcpy(&sockaddr.sin_addr.s_addr, &answer.data[0], 4);
            result.emplace_back(std::make_shared<IPv4Address>(sockaddr));
        } else if (answer.type == TYPE_AAAA || answer.data_length == 16) {
            sockaddr_in6 sockaddr{};
            sockaddr.sin6_family = AF_INET6;
            memcpy(&sockaddr.sin6_addr, &answer.data[0], 16);
            result.emplace_back(std::make_shared<IPv6Address>(sockaddr));
        } else {
            continue;
        }
    }
    return result;
}
