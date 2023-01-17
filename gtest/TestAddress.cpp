#include "sese/net/AddressPool.h"
#include "sese/record/LogHelper.h"
#include "gtest/gtest.h"

TEST(TestAddress, AddressLookUp) {
    sese::record::LogHelper log("AddressLookUp");
    auto address = sese::Address::lookUpAny("www.baidu.com");
    ASSERT_TRUE(address != nullptr);
    log.info("www.baidu.com: %s", address->getAddress().c_str());
}

TEST(TestAddress, IPv6) {
    sese::record::LogHelper log("IPv6");
    auto address6 = sese::IPv6Address::create("fe80::ce6:3cc:f93a:4203", 0);
    ASSERT_TRUE(address6 != nullptr);
    log.info("%s", address6->getAddress().c_str());
}

TEST(TestAddress, AddressPool) {
    auto address0 = sese::IPv4AddressPool::lookup("www.baidu.com");
    auto address1 = sese::IPv4AddressPool::lookup("www.baidu.com");
    ASSERT_TRUE(address0.get() == address1.get());
}