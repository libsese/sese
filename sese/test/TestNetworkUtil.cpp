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
#include "gtest/gtest.h"

TEST(TestNetworkUtil, Interface) {
    auto interfaces = sese::system::NetworkUtil::getNetworkInterface();
    for (decltype(auto) inter: interfaces) {
        printf("Name: %s\n", inter.name.c_str());
        printf("Mac: %02X-%02X-%02X-%02X-%02X-%02X\n",
               inter.mac[0],
               inter.mac[1],
               inter.mac[2],
               inter.mac[3],
               inter.mac[4],
               inter.mac[5]
        );

        printf("IPv4 Address:\n");
        for (decltype(auto) addr: inter.ipv4Addresses) {
            puts(addr->getAddress().c_str());
        }

        printf("IPv6 Address:\n");
        for (decltype(auto) addr: inter.ipv6Addresses) {
            puts(addr->getAddress().c_str());
        }

        printf("\n");
    }
}