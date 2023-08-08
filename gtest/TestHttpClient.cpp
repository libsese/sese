#include <sese/net/http/HttpClient.h>
#include <sese/record/Marco.h>
#include <sese/util/Util.h>

#include <gtest/gtest.h>

TEST(TestHttpClient, UrlParser_0) {
    auto client = sese::net::http::HttpClient::create("https://localhost:8090/home.html");
    ASSERT_NE(client, nullptr);

    decltype(auto) req = client->getRequest();
    decltype(auto) resp = client->getResponse();

    ASSERT_EQ(client->address->getPort(), 8090);
    ASSERT_EQ(req.getUrl(), "/home.html");
}

TEST(TestHttpClient, UrlParser_1) {
    auto client = sese::net::http::HttpClient::create("http://192.168.3.200:8000/index.html?user=foo");
    ASSERT_NE(client, nullptr);

    decltype(auto) req = client->getRequest();
    decltype(auto) resp = client->getResponse();

    ASSERT_EQ(client->address->getPort(), 8000);
    ASSERT_EQ(req.getUrl(), "/index.html?user=foo");
}

TEST(TestHttpClient, UrlParser_2) {
    auto client = sese::net::http::HttpClient::create("https://www.baidu.com.cn/");
    ASSERT_NE(client, nullptr);

    decltype(auto) req = client->getRequest();
    decltype(auto) resp = client->getResponse();

    ASSERT_EQ(client->address->getPort(), 443);
    ASSERT_EQ(req.getUrl(), "/");
}

TEST(TestHttpClient, UrlParser_3) {
    auto client = sese::net::http::HttpClient::create("https://www.baidu.com:cn/");
    ASSERT_EQ(client, nullptr);
}

TEST(TestHttpClient, UrlParser_4) {
    auto client = sese::net::http::HttpClient::create("https://www.baidu:com:cn/");
    ASSERT_EQ(client, nullptr);
}

TEST(TestHttpClient, UrlParser_5) {
    auto client = sese::net::http::HttpClient::create("https://host/");
    ASSERT_EQ(client, nullptr);
}

TEST(TestHttpClient, SSL_NO_KEEPALIVE) {
    auto client = sese::net::http::HttpClient::create("https://bing.com/index.html");
    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();

    decltype(auto) resp = client->getResponse();
    SESE_INFO("status code %d", resp.getCode());
    for (decltype(auto) item: resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }

    auto len = std::atol(client->resp.get("content-length", "0").c_str());
    char buffer[1024]{};
    if (len) {
        ASSERT_EQ(client->getResponse().getBody().read(buffer, len), len);
        SESE_INFO("content:\n%s", buffer);
    }
}

TEST(TestHttpClient, SSL_KEEPALIVE) {
    auto client = sese::net::http::HttpClient::create("https://www.baidu.com/index.html", true);
    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();

    decltype(auto) resp = client->getResponse();
    SESE_INFO("1st status code %d", resp.getCode());
    for (decltype(auto) item: resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }

    auto len = std::atol(client->resp.get("content-length", "0").c_str());
    if (len != 0) {
        char buffer[1024];
        while (true) {
            auto read = client->getResponse().getBody().read(buffer, 1024);
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
    SESE_INFO("2rd status code %d", resp.getCode());
    for (decltype(auto) item: resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }

    for (decltype(auto) cookie: *resp.getCookies()) {
        SESE_DEBUG("Cookie %s:%s", cookie.second->getName().c_str(), cookie.second->getValue().c_str());
    }
}

TEST(TestHttpClient, NO_SSL_NO_KEEPALIVE) {
    auto client = sese::net::http::HttpClient::create("http://bing.com/index.html");
    decltype(auto) req = client->getRequest();
    decltype(auto) resp = client->getResponse();

    req.setType(sese::net::http::RequestType::Options);

    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();

    SESE_INFO("status code %d", resp.getCode());
    for (decltype(auto) item: resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }

    auto len = std::atol(client->resp.get("content-length", "0").c_str());
    char buffer[1024]{};
    if (len) {
        client->getResponse().getBody().read(buffer, len);
        SESE_INFO("content:\n%s", buffer);
    }
}

TEST(TestHttpClient, NO_SSL_KEEPALIVE) {
    auto client = sese::net::http::HttpClient::create("http://www.baidu.com/index.html", true);
    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();

    decltype(auto) resp = client->getResponse();
    SESE_INFO("1st status code %d", resp.getCode());
    for (decltype(auto) item: resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }
    for (decltype(auto) cookie: *resp.getCookies()) {
        SESE_DEBUG("Cookie %s:%s", cookie.second->getName().c_str(), cookie.second->getValue().c_str());
    }

    auto len = std::atol(client->resp.get("content-length", "0").c_str());
    if (len != 0) {
        char buffer[1024];
        while (true) {
            auto read = client->getResponse().getBody().read(buffer, 1024);
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
    req.setType(sese::net::http::RequestType::Put);
    client->getRequest().getBody().write("Hello", 5);

    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();
    SESE_INFO("2rd status code %d", resp.getCode());
    for (decltype(auto) item: resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }
    for (decltype(auto) cookie: *resp.getCookies()) {
        SESE_DEBUG("Cookie %s:%s", cookie.second->getName().c_str(), cookie.second->getValue().c_str());
    }
}