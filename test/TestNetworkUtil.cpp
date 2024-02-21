#include "sese/system/NetworkUtil.h"
#include "gtest/gtest.h"

TEST(TestNetworkUtil, Interface) {
    auto interfaces = sese::system::NetworkUtil::getNetworkInterface();
    for (decltype(auto) Interface: interfaces) {
        printf("Name: %s\n", Interface.name.c_str());
        printf("Mac: %02X-%02X-%02X-%02X-%02X-%02X\n",
               Interface.mac[0],
               Interface.mac[1],
               Interface.mac[2],
               Interface.mac[3],
               Interface.mac[4],
               Interface.mac[5]
        );

        printf("IPv4 Address:\n");
        for (decltype(auto) addr: Interface.ipv4Addresses) {
            puts(addr->getAddress().c_str());
        }

        printf("IPv6 Address:\n");
        for (decltype(auto) addr: Interface.ipv6Addresses) {
            puts(addr->getAddress().c_str());
        }

        printf("\n");
    }
}