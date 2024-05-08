#include <gtest/gtest.h>
#include <sese/net/http/RequestableFactory.h>
#include <sese/thread/Async.h>
#include <sese/record/Marco.h>

using namespace sese::net::http;

#define ASSERT_NOT_NULL(x) ASSERT_TRUE(x != nullptr)

TEST(TestHttpClient, WithoutSSL) {
    auto client = RequestableFactory::createHttpRequest("http://www.baidu.com");
    ASSERT_NOT_NULL(client);
    ASSERT_TRUE(client->request()) << client->getLastError();

    for (auto &&[key, value]: *client->getResponse()) {
        SESE_INFO("%s: %s", key.c_str(), value.c_str());
    }
}

TEST(TestHttpClient, WithSSL) {
    using namespace std::chrono_literals;

    auto client = RequestableFactory::createHttpRequest("https://www.baidu.com");
    ASSERT_NOT_NULL(client);
    // ASSERT_TRUE(client->request()) << client->getLastError();
    auto future = sese::async<bool>([&]{return client->request();});
    std::future_status status;
    do {
        status = future.wait_for(0.1s);
    } while (status != std::future_status::ready);

    for (auto &&[key, value]: *client->getResponse()) {
        SESE_INFO("%s: %s", key.c_str(), value.c_str());
    }
}

TEST(TestHttpClient, DISABLED_WithProxy) {
    auto client = RequestableFactory::createHttpRequest("https://www.baidu.com", "http://127.0.0.1:7890");
    ASSERT_NOT_NULL(client);
    ASSERT_TRUE(client->request()) << client->getLastError();

    for (auto &&[key, value]: *client->getResponse()) {
        SESE_INFO("%s: %s", key.c_str(), value.c_str());
    }
}

// todo HttpClient 增加发送缓存
TEST(TestHttpClient, DISABLED_KeepAlive) {
    auto client = RequestableFactory::createHttpRequest("https://www.baidu.com");
    ASSERT_NOT_NULL(client);
    client->getRequest()->setType(RequestType::HEAD);
    ASSERT_TRUE(client->request()) << client->getLastError();

    SESE_INFO("first request.");
    for (auto &&[key, value]: *client->getResponse()) {
        SESE_INFO("%s: %s", key.c_str(), value.c_str());
    }

    client->getRequest()->setUrl("/index.html");
    ASSERT_TRUE(client->request()) << client->getLastError() << client->getLastErrorString();

    SESE_INFO("second request.");
    for (auto &&[key, value]: *client->getResponse()) {
        SESE_INFO("%s: %s", key.c_str(), value.c_str());
    }
}