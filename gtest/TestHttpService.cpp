#include <sese/service/HttpService_V1.h>
#include <sese/service/BalanceLoader.h>
#include <sese/security/SSLContextBuilder.h>
#include <sese/net/http/HttpClient.h>
#include <sese/record/Marco.h>
#include <sese/util/Util.h>
#include <sese/thread/Async.h>

#include <random>

#include <gtest/gtest.h>

#define printf SESE_INFO

using namespace std::chrono_literals;

static sese::net::IPv4Address::Ptr createAddress() {
    auto port = sese::net::createRandomPort();
    printf("select port %d\n", (int) port);
    return sese::net::IPv4Address::create("127.0.0.1", port);
}

// 重定向
void redirect(
        sese::net::http::Request &req,
        sese::net::http::Response &resp
) noexcept {
    SESE_INFO("req GET: %s", req.getUrl().c_str());
    resp.setCode(301);
    resp.set("Location", "/LICENSE");
}

// post 测试
void post(
        sese::net::http::Request &req,
        sese::net::http::Response &resp
) noexcept {
    char *end;
    auto len = std::strtol(req.get("Content-Length", "0").c_str(), &end, 0);
    if (len != 0) {
        char b[1024]{};
        req.getBody().read(b, len);
        SESE_INFO("req POST: %s\n%s", req.getUrl().c_str(), b);
    } else {
        SESE_INFO("req POST: %s", req.getUrl().c_str());
    }

    std::string content = "Hello Client";
    resp.setCode(200);
    resp.getBody().write(content.c_str(), content.length());
}

TEST(TestHttpService, SSL_KEEPALIVE) {
    auto servCtx = sese::security::SSLContextBuilder::SSL4Server();
    servCtx->importCertFile(PROJECT_PATH "/gtest/Data/test-ca.crt");
    servCtx->importPrivateKeyFile(PROJECT_PATH "/gtest/Data/test-key.pem");
    ASSERT_TRUE(servCtx->authPrivateKey());

    auto addr = createAddress();

    sese::service::v1::HttpConfig config;
    config.servName = "Server for Test";
    config.servCtx = servCtx;
    config.workDir = PROJECT_PATH;
    config.keepalive = 30;

    config.setController("/", redirect);
    config.setController("/post", post);

    sese::service::BalanceLoader service;
    service.setThreads(4);
    service.setAddress(addr);
    ASSERT_TRUE(service.init<sese::service::v1::HttpService>([&config]() -> auto {
        return new sese::service::v1::HttpService(&config);
    }));
    service.start();
    ASSERT_TRUE(service.isStarted());

    auto client = sese::net::http::HttpClient::create("https://localhost:" + std::to_string(addr->getPort()) + "/", true);
    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();
    for (decltype(auto) item: client->resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }

    std::string content = "Hello Server";
    client->req.setUrl("/post");
    client->req.setType(sese::net::http::RequestType::Post);
    client->req.getBody().write(content.c_str(), content.length());

    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();

    for (decltype(auto) item: client->resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }
    auto len = std::atol(client->resp.get("content-length", "0").c_str());
    char buffer[MTU_VALUE]{};
    client->resp.getBody().read(buffer, len);
    SESE_INFO("content: %s", buffer);

    service.stop();
}

TEST(TestHttpService, NO_SSL_KEEPALIVE) {
    auto addr = createAddress();


    auto group = sese::net::http::ControllerGroup("/my_group");
    group.setController("/", redirect);
    group.setController("/post", post);

    sese::service::v1::HttpConfig config;
    config.servName = "Server for Test";
    config.workDir = PROJECT_PATH;
    config.keepalive = 30;
    config.setController(group);

    sese::service::BalanceLoader service;
    service.setThreads(4);
    service.setAddress(addr);
    ASSERT_TRUE(service.init<sese::service::v1::HttpService>([&config]() -> auto {
        return new sese::service::v1::HttpService(&config);
    }));
    service.start();
    ASSERT_TRUE(service.isStarted());

    auto client = sese::net::http::HttpClient::create("http://localhost:" + std::to_string(addr->getPort()) + "/my_group/", true);
    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();

    for (decltype(auto) item: client->resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }

    std::string content = "Hello Server";
    client->req.setUrl("/my_group/post");
    client->req.setType(sese::net::http::RequestType::Post);
    client->req.getBody().write(content.c_str(), content.length());

    auto future = sese::async<bool>([&]() {
        return client->doRequest();
    });

    std::future_status status;
    do {
        SESE_INFO("no ready");
        status = future.wait_for(100ms);
    } while (status != std::future_status::ready);

    ASSERT_TRUE(future.get()) << sese::net::getNetworkError();
    for (decltype(auto) item: client->resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }
    auto len = std::atol(client->resp.get("content-length", "0").c_str());
    char buffer[MTU_VALUE]{};
    client->resp.getBody().read(buffer, len);
    SESE_INFO("content: %s", buffer);

    service.stop();
}