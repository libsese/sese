#include <sese/net/http/HttpClient.h>
#include <sese/record/Marco.h>
#include <sese/util/Util.h>

#include <gtest/gtest.h>

TEST(TestHttpClient, UrlParser_0) {
    auto client = sese::net::http::HttpClient::create("https://localhost:8090/home.html");
    ASSERT_NE(client, nullptr);

    decltype(auto) req = client->getRequest();
    // decltype(auto) resp = client->getResponse();

    ASSERT_EQ(client->address->getPort(), 8090);
    ASSERT_EQ(req->getUrl(), "/home.html");
}

TEST(TestHttpClient, UrlParser_1) {
    auto client = sese::net::http::HttpClient::create("http://192.168.3.200:8000/index.html?user=foo");
    ASSERT_NE(client, nullptr);

    decltype(auto) req = client->getRequest();
    // decltype(auto) resp = client->getResponse();

    ASSERT_EQ(client->address->getPort(), 8000);
    ASSERT_EQ(req->getUrl(), "/index.html?user=foo");
}

TEST(TestHttpClient, UrlParser_2) {
    auto client = sese::net::http::HttpClient::create("https://www.baidu.com.cn/");
    ASSERT_NE(client, nullptr);

    decltype(auto) req = client->getRequest();
    // decltype(auto) resp = client->getResponse();

    ASSERT_EQ(client->address->getPort(), 443);
    ASSERT_EQ(req->getUrl(), "/");
}

TEST(TestHttpClient, UrlParser_3) {
    auto client = sese::net::http::HttpClient::create("https://www.baidu.com:cn/");
    ASSERT_EQ(client, nullptr);
}

TEST(TestHttpClient, UrlParser_4) {
    auto client = sese::net::http::HttpClient::create("https://www.baidu:com:cn/");
    ASSERT_EQ(client, nullptr);
}

TEST(TestHttpClient, BLOCKING_SSL_NO_KEEPALIVE) {
    auto client = sese::net::http::HttpClient::create("https://microsoft.com/index.html");
    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();

    decltype(auto) resp = client->getResponse();
    SESE_INFO("status code %d", resp->getCode());
    for (decltype(auto) item: *resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }

    // auto len = std::atol(client->resp.get("content-length", "0").c_str());
    // char buffer[10240]{};
    // if (len) {
    //     ASSERT_EQ(client->getResponse().getBody().read(buffer, len), len);
    //     SESE_INFO("content:\n%s", buffer);
    // }
}

TEST(TestHttpClient, BLOCKING_SSL_KEEPALIVE) {
    auto client = sese::net::http::HttpClient::create("https://microsoft.com/", true);
    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();

    decltype(auto) resp = client->getResponse();
    SESE_INFO("1st status code %d", resp->getCode());
    for (decltype(auto) item: *resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }

    auto len = std::atol(client->resp->get("content-length", "0").c_str());
    if (len != 0) {
        char buffer[1024];
        while (true) {
            auto read = client->getResponse()->getBody().read(buffer, 1024);
            if (read > 0) {
                len -= (int) read;
                if (len == 0) {
                    break;
                }
            } else {
                FAIL();
            }
        }
    }

    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();
    SESE_INFO("2rd status code %d", resp->getCode());
    for (decltype(auto) item: *resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }

    for (decltype(auto) cookie: *resp->getCookies()) {
        SESE_DEBUG("Cookie %s:%s", cookie.second->getName().c_str(), cookie.second->getValue().c_str());
    }
}

TEST(TestHttpClient, BLOCKING_NO_SSL_NO_KEEPALIVE) {
    auto client = sese::net::http::HttpClient::create("http://microsoft.com/index.html");
    decltype(auto) req = client->getRequest();
    decltype(auto) resp = client->getResponse();

    req->setType(sese::net::http::RequestType::Options);

    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();

    SESE_INFO("status code %d", resp->getCode());
    for (decltype(auto) item: *resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }

    auto len = std::atol(client->resp->get("content-length", "0").c_str());
    char buffer[1024]{};
    if (len) {
        client->getResponse()->getBody().read(buffer, len);
        SESE_INFO("content:\n%s", buffer);
    }
}

TEST(TestHttpClient, BLOCKING_NO_SSL_KEEPALIVE) {
    auto client = sese::net::http::HttpClient::create("http://microsoft.com/", true);
    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();

    decltype(auto) resp = client->getResponse();
    SESE_INFO("1st status code %d", resp->getCode());
    for (decltype(auto) item: *resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }
    for (decltype(auto) cookie: *resp->getCookies()) {
        SESE_DEBUG("Cookie %s:%s", cookie.second->getName().c_str(), cookie.second->getValue().c_str());
    }

    auto len = std::atol(client->resp->get("content-length", "0").c_str());
    if (len != 0) {
        char buffer[1024];
        while (true) {
            auto read = client->getResponse()->getBody().read(buffer, 1024);
            if (read > 0) {
                len -= (int) read;
                if (len == 0) {
                    break;
                }
            } else {
                FAIL();
            }
        }
    }

    decltype(auto) req = client->getRequest();
    req->setType(sese::net::http::RequestType::Put);
    client->getRequest()->getBody().write("Hello", 5);

    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();
    SESE_INFO("2rd status code %d", resp->getCode());
    for (decltype(auto) item: *resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }
    for (decltype(auto) cookie: *resp->getCookies()) {
        SESE_DEBUG("Cookie %s:%s", cookie.second->getName().c_str(), cookie.second->getValue().c_str());
    }
}

#include <sese/service/http/HttpClient_V1.h>

class TestAsyncHttpClient : public testing::Test {
public:
    using Client = sese::service::v1::HttpClient;
    using Handle = sese::service::v1::HttpClientHandle;

protected:
    static Client client;

    static void SetUpTestSuite() {
        client.setThreads(2);
        ASSERT_TRUE(client.init());
    }

    static void TearDownTestSuite() {
        client.shutdown();
    }
};

TestAsyncHttpClient::Client TestAsyncHttpClient::client;

TEST_F(TestAsyncHttpClient, DISABLED_Once) {
    auto handle = Handle::create("https://microsoft.com");
    handle->writeBody("Hello", 5);
    auto future = client.post(handle);
    auto status = future.get();
    if (status == Handle::RequestStatus::Succeeded) {
        SESE_INFO("The request succeeded with %d.", handle->getStatusCode());
        handle->responseForEach([](const sese::service::v1::HttpClientHandle::HeaderForEachFunctionArgs &pair) {
            SESE_INFO("%s: %s", pair.first.c_str(), pair.second.c_str());
        });
    } else {
        FAIL();
    }
}

#include <sese/security/SSLContextBuilder.h>

TEST_F(TestAsyncHttpClient, DISABLED_Multi) {
    // auto address = sese::net::IPv4Address::create("110.242.68.66", 80);
    auto address = std::dynamic_pointer_cast<sese::net::IPv4Address>(sese::net::IPv4Address::lookUpAny("microsoft.com"));
    address->setPort(443);
    auto context = sese::security::SSLContextBuilder::SSL4Client();

    auto handle1 = Handle::create("https://bing.com");
    auto handle2 = Handle::create(address, context);

    auto f1 = client.post(handle1);
    auto f2 = client.post(handle2);
    auto s1 = f1.get();
    auto s2 = f2.get();

    if (s1 == Handle::RequestStatus::Succeeded) {
        SESE_INFO("The handle1 succeeded with %d.", handle1->getStatusCode());
        handle1->responseForEach([](const sese::service::v1::HttpClientHandle::HeaderForEachFunctionArgs &pair) {
            SESE_INFO("%s: %s", pair.first.c_str(), pair.second.c_str());
        });
    } else {
        FAIL() << sese::net::getNetworkErrorString();
    }

    if (s2 == Handle::RequestStatus::Succeeded) {
        SESE_INFO("The handle2 succeeded with %d.", handle2->getStatusCode());
        handle2->responseForEach([](const sese::service::v1::HttpClientHandle::HeaderForEachFunctionArgs &pair) {
            SESE_INFO("%s: %s", pair.first.c_str(), pair.second.c_str());
        });
    } else {
        FAIL() << sese::net::getNetworkErrorString();
    }
}

TEST_F(TestAsyncHttpClient, DISABLED_KeepAlive) {
    auto handle = Handle::create("http://bing.com");
    auto future = client.post(handle);
    auto status = future.get();
    if (status == Handle::RequestStatus::Succeeded) {
        SESE_INFO("The request succeeded with %d.", handle->getStatusCode());
        handle->responseForEach([](const sese::service::v1::HttpClientHandle::HeaderForEachFunctionArgs &pair) {
            SESE_INFO("%s: %s", pair.first.c_str(), pair.second.c_str());
        });
    } else {
        FAIL();
    }

    handle->setUrl("/index.html");
    future = client.post(handle);
    status = future.get();
    if (status == Handle::RequestStatus::Succeeded) {
        SESE_INFO("The request succeeded with %d.", handle->getStatusCode());
        handle->responseForEach([](const sese::service::v1::HttpClientHandle::HeaderForEachFunctionArgs &pair) {
            SESE_INFO("%s: %s", pair.first.c_str(), pair.second.c_str());
        });
    } else {
        FAIL();
    }
}