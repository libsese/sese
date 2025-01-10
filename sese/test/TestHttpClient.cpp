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

#include <gtest/gtest.h>
#include <sese/net/http/HttpClient.h>
#include <sese/thread/Async.h>
#include <sese/record/Marco.h>

using namespace sese::net::http;

#define ASSERT_NOT_NULL(x) ASSERT_TRUE(x != nullptr)

TEST(TestHttpClient, WithoutSSL) {
    auto client = HttpClient::create("http://www.baidu.com");
    ASSERT_NOT_NULL(client);
    ASSERT_TRUE(client->request()) << client->getLastError();

    for (auto &&[key, value]: *client->getResponse()) {
        SESE_INFO("{}: {}", key, value);
    }
}

TEST(TestHttpClient, WithSSL) {
    using namespace std::chrono_literals;

    auto client = HttpClient::create("https://www.baidu.com");
    ASSERT_NOT_NULL(client);
    // ASSERT_TRUE(client->request()) << client->getLastError();
    auto future = sese::async<bool>([&]{return client->request();});
    std::future_status status;
    do {
        status = future.wait_for(0.1s);
    } while (status != std::future_status::ready);

    for (auto &&[key, value]: *client->getResponse()) {
        SESE_INFO("{}: {}", key, value);
    }
}

TEST(TestHttpClient, DISABLED_WithProxy) {
    auto client = HttpClient::create("https://www.baidu.com", "http://127.0.0.1:7890");
    ASSERT_NOT_NULL(client);
    ASSERT_TRUE(client->request()) << client->getLastError();

    for (auto &&[key, value]: *client->getResponse()) {
        SESE_INFO("{}: {}", key, value);
    }
}

TEST(TestHttpClient, KeepAlive) {
    auto client = HttpClient::create("https://www.baidu.com");
    ASSERT_NOT_NULL(client);
    client->getRequest()->setType(RequestType::HEAD);
    ASSERT_TRUE(client->request()) << client->getLastError() << client->getLastErrorString();

    SESE_INFO("first request.");
    for (auto &&[key, value]: *client->getResponse()) {
        SESE_INFO("{}: {}", key, value);
    }

    client->getRequest()->setUrl("/index.html");
    ASSERT_TRUE(client->request()) << client->getLastError() << client->getLastErrorString();

    SESE_INFO("second request.");
    for (auto &&[key, value]: *client->getResponse()) {
        SESE_INFO("{}: {}", key, value);
    }
}