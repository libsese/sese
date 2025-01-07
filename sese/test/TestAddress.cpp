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

#include "sese/net/AddressPool.h"
#include "sese/record/Marco.h"
#include "gtest/gtest.h"

TEST(TestAddress, AddressLookUp) {
    auto address = sese::net::Address::lookUpAny("www.baidu.com");
    ASSERT_TRUE(address != nullptr);
    SESE_INFO("www.baidu.com: {}", address->getAddress());
}

TEST(TestAddress, IPv6) {
    auto address6 = sese::net::IPv6Address::create("fe80::ce6:3cc:f93a:4203", 0);
    ASSERT_TRUE(address6 != nullptr);
    address6->setPort(8080);
    EXPECT_EQ(address6->getPort(), 8080);
    EXPECT_EQ(address6->getFamily(), AF_INET6);
    SESE_INFO("{}", address6->getAddress());
}

TEST(TestAddress, AddressPool) {
    auto address0 = sese::net::IPv4AddressPool::lookup("www.baidu.com");
    auto address1 = sese::net::IPv4AddressPool::lookup("www.baidu.com");
    ASSERT_TRUE(address0.get() == address1.get());

    ASSERT_EQ(sese::net::IPv4AddressPool::lookup(".com"), nullptr);
}

TEST(TestAddress, IPAddress) {
    auto address0 = sese::net::IPAddress::create("127.0.0.1");
    ASSERT_NE(address0, nullptr);

    auto address1 = sese::net::IPAddress::create("256.0.0.1");
    ASSERT_EQ(address1, nullptr);
}

TEST(TestAddress, IPv4Address) {
    auto any = sese::net::IPv4Address::any();
    ASSERT_NE(any, nullptr);
    SESE_INFO("any: {}", any->getAddress());

    auto local = sese::net::IPv4Address::localhost();
    ASSERT_NE(local, nullptr);
    SESE_INFO("local: {}", local->getAddress());

    auto address0 = sese::net::IPv4Address::create("256.0.0.1", 0);
    ASSERT_EQ(address0, nullptr);

    auto address1 = local->getBroadcastAddress(33);
    ASSERT_EQ(address1, nullptr);

    auto address2 = local->getBroadcastAddress(24);
    ASSERT_NE(address2, nullptr);
    SESE_INFO("local broadcast address: {}", address2->getAddress());

    auto address3 = local->getNetworkAddress(33);
    ASSERT_EQ(address3, nullptr);

    auto address4 = local->getNetworkAddress(24);
    ASSERT_NE(address4, nullptr);
    SESE_INFO("local network address: {}", address4->getAddress());

    auto address5 = local->getSubnetMask(33);
    ASSERT_EQ(address5, nullptr);

    auto address6 = local->getSubnetMask(24);
    ASSERT_NE(address6, nullptr);
    SESE_INFO("local subnet mask: {}", address6->getAddress());
}

TEST(TestAddress, IPv6Address) {
    auto any = sese::net::IPv6Address::any();
    ASSERT_NE(any, nullptr);
    SESE_INFO("any: {}", any->getAddress());

    auto local = sese::net::IPv6Address::localhost();
    ASSERT_NE(local, nullptr);
    SESE_INFO("local: {}", local->getAddress());

    auto address0 = sese::net::IPv6Address::create("256.0.0.1", 0);
    ASSERT_EQ(address0, nullptr);

    auto address1 = local->getBroadcastAddress(10);
    ASSERT_NE(address1, nullptr);
    SESE_INFO("local broadcast address: {}", address1->getAddress());

    auto address2 = local->getNetworkAddress(10);
    ASSERT_NE(address2, nullptr);
    SESE_INFO("local network address: {}", address2->getAddress());

    auto address3 = local->getSubnetMask(10);
    ASSERT_NE(address3, nullptr);
    SESE_INFO("local subnet mask: {}", address3->getAddress());
}

TEST(TestAddress, Family) {
    auto ip = sese::net::IPv4Address::localhost();
    ip->setFamily(123);
    EXPECT_EQ(ip->getFamily(), 123);

    auto ipv6 = sese::net::IPv6Address::localhost();
    ipv6->setFamily(96);
    EXPECT_EQ(ipv6->getFamily(), 96);
}