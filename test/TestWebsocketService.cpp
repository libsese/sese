#include <sese/service/BalanceLoader.h>
#include <sese/system/Process.h>
#include <sese/record/Marco.h>
#include <sese/net/ws/WebsocketAuthenticator.h>

#include <random>

#include <gtest/gtest.h>

#define printf SESE_INFO

static sese::net::IPv4Address::Ptr createAddress() {
    auto port = sese::net::createRandomPort();
    printf("select port %d", (int) port);
    return sese::net::IPv4Address::create("127.0.0.1", port);
}

TEST(TestWebsocket, Auth_0) {
    auto key = "dGhlIHNhbXBsZSBub25jZQ==";
    auto result = sese::net::ws::WebsocketAuthenticator::toResult(key);
    EXPECT_EQ(std::strcmp("s3pPLMBiTxaQ9kYGzzhZRbK+xOo=", result.get()), 0);
}

TEST(TestWebsocket, Auth_1) {
    auto pair = sese::net::ws::WebsocketAuthenticator::generateKeyPair();
    EXPECT_TRUE(sese::net::ws::WebsocketAuthenticator::verify(pair.first.get(), pair.second.get()));
}