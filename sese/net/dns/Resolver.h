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

#include <sese/net/IPv6Address.h>

#include <vector>
#include <random>

namespace sese::net::dns {
class Resolver {
    std::vector<IPAddress::Ptr> name_servers;

    std::random_device device;
    std::mt19937 generator;

    std::vector<IPAddress::Ptr> resolve(const IPAddress::Ptr &name_server, const std::string &hostname, uint16_t type);
public:
    Resolver();

    bool addNameServer(const std::string &ip, uint16_t port = 53);

    void addNameServer(const IPAddress::Ptr &ip_address);

    std::vector<IPAddress::Ptr> resolve(const std::string &hostname, uint16_t type);
};
} // namespace sese::net::dns