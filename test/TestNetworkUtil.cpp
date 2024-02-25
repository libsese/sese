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