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

#include "sese/system/NetworkUtil.h"
#include "sese/net/IPv6Address.h"
#include <vector>

using namespace sese::system;

#include <map>
#include <ifaddrs.h>
#include <net/if_dl.h>

std::vector<NetworkInterface> NetworkUtil::getNetworkInterface() noexcept {
    std::vector<NetworkInterface> interfaces;
    std::map<std::string, NetworkInterface> map;
    struct ifaddrs *address = nullptr;

    getifaddrs(&address);

    while (address) {
        if (address->ifa_addr == nullptr) {
            address = address->ifa_next;
            continue;
        }

        auto iterator = map.find(address->ifa_name);
        if (iterator == map.end()) {
            auto i = NetworkInterface();
            iterator = map.insert({address->ifa_name, i}).first;
        }
        if (address->ifa_addr->sa_family == AF_INET) {
            auto iterator = map.find(address->ifa_name);
            sockaddr_in addr = *(sockaddr_in *) (address->ifa_addr);
            iterator->second.ipv4Addresses.emplace_back(std::make_shared<sese::net::IPv4Address>(addr));
        } else if (address->ifa_addr->sa_family == AF_INET6) {
            sockaddr_in6 addr = *(sockaddr_in6 *) (address->ifa_addr);
            iterator->second.ipv6Addresses.emplace_back(std::make_shared<sese::net::IPv6Address>(addr));
        } else if (address->ifa_addr->sa_family == AF_LINK) {
            auto ptr = (unsigned char *) LLADDR((struct sockaddr_dl *) (address)->ifa_addr);
            iterator->second.name = address->ifa_name;
            memcpy(iterator->second.mac.data(), ptr, 6);
        }

        address = address->ifa_next;
    }

    freeifaddrs(address);

    interfaces.reserve(map.size());
    for (decltype(auto) i: map) {
        i.second.name = i.first;
        interfaces.emplace_back(i.second);
    }

    return interfaces;
}