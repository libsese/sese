#include <sese/service/http/HttpService_V2.h>
#include <sese/service/BalanceLoader.h>
#include <sese/security/SSLContextBuilder.h>
#include <sese/net/http/HttpClient.h>
#include <sese/record/Marco.h>
#include <sese/util/Util.h>
#include <sese/io/OutputUtil.h>

#include <random>

#include <gtest/gtest.h>

#define printf SESE_INFO

using namespace std::chrono_literals;

static sese::net::IPv4Address::Ptr createAddress() {
    auto port = sese::net::createRandomPort();
    printf("select port %d", (int) port);
    return sese::net::IPv4Address::create("127.0.0.1", port);
}

static void ControllerIndex(
        sese::net::http::Request &req,
        sese::net::http::Response &resp
) {
    resp.set("message", "hello");
    resp.getBody() << "Hello World";
}

static void ControllerPost(
        sese::net::http::Request &req,
        sese::net::http::Response &resp
) {
    char buffer[1024]{};
    req.getBody().read(buffer, sizeof(buffer));
    printf("body: %s", buffer);
    resp.setCode(200);
    resp.getBody() << "OK";
}

TEST(TestService, SSL_KEEPALIVE_V2) {
    auto servCtx = sese::security::SSLContextBuilder::SSL4Server();
    servCtx->importCertFile(PROJECT_PATH "/gtest/Data/test-ca.crt");
    servCtx->importPrivateKeyFile(PROJECT_PATH "/gtest/Data/test-key.pem");
    ASSERT_TRUE(servCtx->authPrivateKey());

    auto addr = createAddress();

    sese::service::v2::HttpConfig config;
    config.setServCtx(servCtx);
    config.setServName("my service");
    config.setKeepAlive(30);
    config.addExcludeIndexedHeader("message");
    config.setWorkDir("C:/Users/kaoru/Desktop/www");
    config.setController("/", ControllerIndex);
    config.setController("/post", ControllerPost);

    sese::service::BalanceLoader service;

    service.setThreads(4);
    service.setAddress(addr);
    ASSERT_TRUE(service.init<sese::service::v2::HttpService>([&config]() -> auto {
        return new sese::service::v2::HttpService(&config);
    }));
    service.start();
    ASSERT_TRUE(service.isStarted());

    auto client = sese::net::http::HttpClient::create("https://localhost:" + std::to_string(addr->getPort()) + "/", true);
    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();
    for (decltype(auto) item: *client->resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }

    std::string content = "Hello Server V2";
    client->req->setUrl("/post");
    client->req->setType(sese::net::http::RequestType::Post);
    client->req->getBody().write(content.c_str(), content.length());

    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();

    for (decltype(auto) item: *client->resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }
    auto len = std::atol(client->resp->get("content-length", "0").c_str());
    char buffer[MTU_VALUE]{};
    client->resp->getBody().read(buffer, len);
    SESE_INFO("content: %s", buffer);

    service.stop();
}