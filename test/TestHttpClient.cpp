#include <gtest/gtest.h>
#include <sese/net/http/RequestableFactory.h>
#include <sese/record/Marco.h>

using namespace sese::net::http;

#define ASSERT_NOT_NULL(x) ASSERT_TRUE(x != nullptr)

TEST(TestHttpClient, RequestWithoutSSL) {
    auto client = RequestableFactory::createHttpRequest("http://www.baidu.com");
    ASSERT_NOT_NULL(client);
    ASSERT_TRUE(client->request()) << client->getLastError();

    for (auto &&[key, value]: *client->getResponse()) {
        SESE_INFO("%s: %s", key.c_str(), value.c_str());
    }
}

TEST(TestHttpClient, RequestWithSSL) {
    auto client = RequestableFactory::createHttpRequest("https://www.baidu.com");
    ASSERT_NOT_NULL(client);
    ASSERT_TRUE(client->request()) << client->getLastError();

    for (auto &&[key, value]: *client->getResponse()) {
        SESE_INFO("%s: %s", key.c_str(), value.c_str());
    }
}

TEST(TestHttpClient, RequestWithProxy) {
    auto client = RequestableFactory::createHttpRequest("https://www.baidu.com", "http://127.0.0.1:7890");
    ASSERT_NOT_NULL(client);
    ASSERT_TRUE(client->request()) << client->getLastError();

    for (auto &&[key, value]: *client->getResponse()) {
        SESE_INFO("%s: %s", key.c_str(), value.c_str());
    }
}