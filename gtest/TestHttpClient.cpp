#include <sese/net/http/HttpClient.h>
#include <sese/record/Marco.h>

#include <gtest/gtest.h>

TEST(TestHttpClient, SSL_NO_KEEPALIVE) {
    auto client = sese::net::http::HttpClient::create("https://bing.com/index.html");
    ASSERT_TRUE(client->doRequest());
    ASSERT_TRUE(client->doResponse());

    decltype(auto) resp = client->getResponse();
    SESE_INFO("status code %d", resp.getCode());
    for (decltype(auto) item: resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }
}