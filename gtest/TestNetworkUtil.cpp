#include "sese/system/NetworkUtil.h"
#include "gtest/gtest.h"

TEST(TestNetworkUtil, Interface) {
    auto interfaces = sese::NetworkUtil::getNetworkInterface();
    for (decltype(auto) Interface : interfaces) {
        printf("Name: %s\n", Interface.name.c_str());
        printf("Mac: ");
        for (decltype(auto) byte : Interface.mac) {
            printf("%02X", byte);
        }
        printf("\n");

        printf("IPv4 Address:\n");
        for(decltype(auto) addr : Interface.ipv4Addresses) {
            puts(addr->getAddress().c_str());
        }

        printf("IPv6 Address:\n");
        for(decltype(auto) addr : Interface.ipv6Addresses) {
            puts(addr->getAddress().c_str());
        }

        printf("\n");
    }
}