#include <sese/service/HttpService.h>
#include <sese/service/BalanceLoader.h>
#include <sese/security/SSLContextBuilder.h>
#include <sese/net/http/HttpClient.h>
#include <sese/record/Marco.h>
#include <sese/util/Util.h>

#include <random>

#include <gtest/gtest.h>

using namespace std::chrono_literals;

sese::net::IPv4Address::Ptr createAddress() {
    std::random_device device;
    auto engine = std::default_random_engine(device());
    std::uniform_int_distribution<uint16_t> dis(1025, 65535);
    auto port = dis(engine);
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

    sese::service::HttpConfig config;
    config.servName = "Server for Test";
    config.servCtx = servCtx;
    config.workDir = PROJECT_PATH;
    config.keepalive = 30;

    config.setController("/", redirect);
    config.setController("/post", post);

    sese::service::BalanceLoader service;
    service.setThreads(4);
    service.setAddress(addr);
    ASSERT_TRUE(service.init<sese::service::HttpService>([&config]() -> auto {
        return new sese::service::HttpService(config);
    }));
    service.start();
    ASSERT_TRUE(service.isStarted());

    auto client = sese::net::http::HttpClient::create("https://localhost:" + std::to_string(addr->getPort()) + "/", true);
    client->makeRequest();
    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();
    sese::sleep(1);
    ASSERT_TRUE(client->doResponse()) << sese::net::getNetworkError();
    for (decltype(auto) item: client->resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }

    std::string content = "Hello Server";
    client->req.setUrl("/post");
    client->req.setType(sese::net::http::RequestType::Post);
    client->req.set("Content-Length", std::to_string(content.length()));
    client->makeRequest();
    client->write(content.c_str(), content.length());
    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();
    sese::sleep(1);
    ASSERT_TRUE(client->doResponse()) << sese::net::getNetworkError();
    for (decltype(auto) item: client->resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }
    auto len = std::atol(client->resp.get("content-length", "0").c_str());
    char buffer[MTU_VALUE]{};
    client->read(buffer, len);
    SESE_INFO("content: %s", buffer);

    service.stop();
}

TEST(TestHttpService, NO_SSL_KEEPALIVE) {
    auto addr = createAddress();


    auto group = sese::net::http::ControllerGroup("/my_group");
    group.setController("/", redirect);
    group.setController("/post", post);

    sese::service::HttpConfig config;
    config.servName = "Server for Test";
    config.workDir = PROJECT_PATH;
    config.keepalive = 30;
    config.setController(group);

    sese::service::BalanceLoader service;
    service.setThreads(4);
    service.setAddress(addr);
    ASSERT_TRUE(service.init<sese::service::HttpService>([&config]() -> auto {
        return new sese::service::HttpService(config);
    }));
    service.start();
    ASSERT_TRUE(service.isStarted());

    auto client = sese::net::http::HttpClient::create("http://localhost:" + std::to_string(addr->getPort()) + "/my_group/", true);
    client->makeRequest();
    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();
    sese::sleep(1);
    ASSERT_TRUE(client->doResponse()) << sese::net::getNetworkError();
    for (decltype(auto) item: client->resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }

    std::string content = "Hello Server";
    client->req.setUrl("/my_group/post");
    client->req.setType(sese::net::http::RequestType::Post);
    client->req.set("Content-Length", std::to_string(content.length()));
    client->makeRequest();
    client->write(content.c_str(), content.length());
    ASSERT_TRUE(client->doRequest()) << sese::net::getNetworkError();
    sese::sleep(1);
    ASSERT_TRUE(client->doResponse()) << sese::net::getNetworkError();
    for (decltype(auto) item: client->resp) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }
    auto len = std::atol(client->resp.get("content-length", "0").c_str());
    char buffer[MTU_VALUE]{};
    client->read(buffer, len);
    SESE_INFO("content: %s", buffer);

    service.stop();
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
#if defined(__linux__) || defined(__APPLE__)
    signal(SIGPIPE, SIG_IGN);
#endif
    return RUN_ALL_TESTS();
}